#!/usr/bin/env -S uv run
# /// script
# requires-python = "==3.11.*"
# dependencies = [
#   "loguru",
#   "plum-dispatch",
#   "pybind11",
#   "ulid2",
#   "pandas",
#   "matplotlib",
#   "seaborn",
#   "pingouin",
#   "rich",
#   "plumbum",
#   "epiclibcpp"
# ]
# ///


import math
import sys
import random
from rich import print
from rich.console import Console
from pathlib import Path
from epiclib import Symbol, Speech_word, Model, Device_base
from epiclib.geometric_utilities import Point
from epiclib import (
    Output_tee,
    Device_exception,
    Coordinator,
    geometric_utilities as gu
)
from epiclib import View_base, Visual_encoder_base, Auditory_encoder_base
from epiclib.pps_globals import PPS_out
from epiclib.output_tee_globals import (Normal_out, Trace_out, Exception_out, Debug_out)
from epiclib import numeric_utilities as nu, random_utilities as ru, symbol_utilities as su
from epiclib.syllable_counter import count_total_syllables
from epiclib.standard_symbols import *
from epiclib.standard_utility_symbols import *
import epiclib.statistics as stats
import epiclib.rms_statistics as rms
from epiclib import raise_device_exception, raise_epic_exception
from epiclib import Coordinator

# print('--hook up output--')

'''
Output_tee only line-buffers for View_base, not for streams.

When you attach a View_base (add_view(View_base*)), Output_tee accumulates text in view_line_buffer and 
calls notify_append_text(...) once per completed line (on '\n' / std::endl).

When you attach a Python stream via add_py_stream, your code mirrors each chunk written by the various << calls 
immediately (no extra buffering). So if you want per-line prefix/postfix on the Python side, you either:

implement that line buffering in your Python writer (your ConsoleWriter v2), or

better: use a Python subclass of View_base so you reuse the C++ line buffering and keep ConsoleWriter simple.
'''

class StreamWriter:
    def __init__(self, prefix: str = '', postfix: str = '', end: str = '\n', stream=None):
        self.prefix = prefix
        self.postfix = postfix
        self.end = end            # what to append per completed line ('' or '\n')
        self.stream = stream or sys.stdout
        self._buf = ''

    def write(self, text, /):
        # accumulate and flush full lines whenever we see '\n'
        self._buf += text
        while True:
            i = self._buf.find('\n')
            if i < 0:
                break
            line, self._buf = self._buf[:i], self._buf[i+1:]
            self.stream.write(f'{self.prefix}{line}{self.postfix}{self.end}')

    def flush(self):
        if self._buf:
            # flush any partial line without appending self.end
            self.stream.write(f'{self.prefix}{self._buf}{self.postfix}')
            self._buf = ''
        self.stream.flush()

    def close(self):
        self.flush()

# class ConsoleWriter:
#     def __init__(self, prefix: str = '', postfix: str = '', end: str = '\n'):
#         self.prefix = prefix
#         self.postfix = postfix
#         self.end = end            # what to append per completed line ('' or '\n')
#         self._buf = ''
#
#     def write(self, text, /):
#         # accumulate and flush full lines whenever we see '\n'
#         self._buf += text
#         while True:
#             i = self._buf.find('\n')
#             if i < 0:
#                 break
#             line, self._buf = self._buf[:i], self._buf[i+1:]
#             print(f'{self.prefix}{line}{self.postfix}', end=self.end)
#
#     def flush(self):
#         if self._buf:
#             # flush any partial line without appending self.end
#             print(f'{self.prefix}{self._buf}{self.postfix}', end='')
#             self._buf = ''
#
#     def close(self):
#         self.flush()


class ConsoleWriter:
    def __init__(self, prefix: str = '', postfix: str = '', end: str = '\n', console: Console | None = None):
        self.prefix = prefix
        self.postfix = postfix
        self.end = end
        self.console = console or Console()
        self._buf = ''

    def write(self, text, /):
        self._buf += text
        while True:
            i = self._buf.find('\n')
            if i < 0:
                break
            line, self._buf = self._buf[:i], self._buf[i+1:]
            self.console.print(
                f'{self.prefix}{line}{self.postfix}',
                end=self.end,
                markup=True,
                highlight=False,
                soft_wrap=False,
            )

    def flush(self):
        if self._buf:
            self.console.print(
                f'{self.prefix}{self._buf}{self.postfix}',
                end='',
                markup=True,
                highlight=False,
                soft_wrap=False,
            )
            self._buf = ''
        # make sure the underlying file is flushed
        try:
            self.console.file.flush()
        except Exception:
            pass

    def close(self):
        self.flush()

print('\n\n--------------------------------------------------')
print('--------------------------------------------------')
print(f'OUTPUT OF {Path(__file__).name}')
print('--------------------------------------------------')
print('--------------------------------------------------\n')

normal_out_writer = ConsoleWriter(prefix='[yellow1]', postfix='[/yellow1]')
trace_out_writer = ConsoleWriter(prefix='[medium_purple3]', postfix='[/medium_purple3]')
pps_out_writer = ConsoleWriter(prefix='[chartreuse1]', postfix='[/chartreuse1]')
debug_out_writer = ConsoleWriter(prefix='[dark_orange]', postfix='[/dark_orange]')

Normal_out.add_py_stream(normal_out_writer)
Trace_out.add_py_stream(trace_out_writer)
PPS_out.add_py_stream(pps_out_writer)
# Debug_out.add_py_stream(debug_out_writer)
Exception_out.add_py_stream(sys.stderr)

Normal_out(f'Writing PI to Normal_out: {str(math.pi)=}\n')
Debug_out(f'Writing PI to Debug_out: {str(math.pi)=}\n')
PPS_out(f'Writing PI to PPS_out: {str(math.pi)=}\n')
Trace_out(f'Writing PI to Trace_out: {str(math.pi)=}\n')

print('--Normal_out--')
print(f'{Normal_out=}')
Normal_out('This is written to Normal_out')  # FIXME: Trying to figure out how to get output in simulation2.py
Trace_out('This is written to Trace_out')

print('--symbol & point--')
sym1 = Symbol('sym1')
sym2 = Symbol('32', True)
print(f'{sym1.has_string_value()=}')
print(f'{sym2.has_string_value()=}')
print(f'{sym1=}')
print(f'{sym2=}')
sym3 = Symbol(Point(32, 44))
print(f'{sym3=}')
print(f'{sym3.get_point()=}')
p = Point(402, 1123)
print(f'{p=}')

print('--Speech_word--')
sw = Speech_word()
sw.name = Symbol('my tone')
sw.location = Point(12.3, 49.7)
print(f"{sw=}", sw)
print(f"{sw.level_left=}")

print('--Device--')


class MyDevice(Device_base):
    def __init__(self, Id, ot):
        super(MyDevice, self).__init__(Id, ot)


device = MyDevice('MyTestDevice', Normal_out)
print(f'{device.processor_info=}')

print('--Model--')
model = Model(device)
print(f'{model.get_time()=}')

print('\n--Parameters--')
# model.describe_parameters()

print('\n--Exceptions--')

exc1 = Device_exception()
exc2 = Device_exception("Some Problem!")
Device_exception("This is it!")

print('--Coordinator--')

# indirect (not working)
# coordinator = Coordinator.get_instance()
# print(f'{coordinator.get_time()=}')

# direct (working)
print(f'{Coordinator.get_time()=}')

print('--Graphics_utilities--')
print(f'{gu.to_radians(3.4343)=}')

cv1 = gu.Cartesian_vector()
cv2 = gu.Cartesian_vector(Point(2.3, 2.3), Point(19.08, 20.77))
print(f'{cv1=}')
print(f'{cv2=}')
print(f'{cv2.delta_x=}')

pv1 = gu.Polar_vector()
pv2 = gu.Polar_vector(Point(2.3, 2.3), Point(19.08, 20.77))
pv3 = gu.Polar_vector(cv2)
print(f'{pv1=}')
print(f'{pv2=}')
print(f'{pv3=}')
print(f'{pv3.theta=}')

ls1 = gu.Line_segment()
ls2 = gu.Line_segment(Point(2.3, 2.3), Point(19.08, 20.77))
print(f'{ls1=}')
print(f'{ls2=}')
print(f'{ls2.get_size()=}')

pg1 = gu.Polygon()
pg2 = gu.Polygon([Point(0, -2), Point(2, 2), Point(-2, 2)])
print(f'{pg1=}')
print(f'{pg2=}')
print(f'{pg2.get_center()=}')
print(f'{pg2.get_vertices()=}')

print('--View_base--')


class MyView(View_base):
    def __init__(self):
        super(MyView, self).__init__()

    def notify_append_text(self, text: str):
        print(f'|||||||||-> {text}')

    def __getattr__(self, name):
        def _missing(*args, **kwargs):
            print("A missing method was called.")
            print("The object was %r, the method was %r. " % (self, name))
            print("It was called with %r and %r as arguments" % (args, kwargs))

        return _missing


vb1 = MyView()
print(f'{vb1=}')

Normal_out.add_view(vb1)
# print(f'{Normal_out.is_present(vb1)=}')
# print(f'{Normal_out.output_enabled()=}')
print(f'Normal_out enabled = {bool(Normal_out)}')
Normal_out("\n\nHello There!\n\n")

print('--Visual_encoder_base--')


class MyVisEncoder(Visual_encoder_base):
    def __init__(self, Id):
        super(MyVisEncoder, self).__init__(Id)


ve1 = MyVisEncoder('TestEncoder')
print(f'{ve1=}')

print('--Auditory_encoder_base--')


class MyAudEncoder(Auditory_encoder_base):
    def __init__(self, Id):
        super(MyAudEncoder, self).__init__(Id)


ae1 = MyAudEncoder('TestEncoder')
print(f'{ae1=}')

print('--Numeric Utilities--')
print(f'{nu.round_to_integer(3.7)=}')  # 4.0
print(f'{nu.int_to_string(123)=}')  # 123
print(f'{nu.time_convert(1, 2, 3)=}')  # 3723000
print(f'{nu.logb2(8)=}')  # 3.0
print(f'{nu.pitch_to_semitones(440)=}')  # 57.01955

print('--Random Utilities--')
p = 0.5
x = 0.5
mean = 0.0
sd = 1.0
base = 0.1
cap = 0.9
lapse_probability = 0.1

print(f'{ru.uniform_detection_function(p)=}')
print(f'{ru.uniform_detection_function(p)=}')
print(f'{ru.gaussian_detection_function(x, mean, sd) =}')
print(f'{ru.gaussian_detection_function(x, mean, sd)=}')
print(f'{ru.gaussian_detection_function(x, mean, sd)=}')
print(f'{ru.gaussian_detection_function(mean, mean, sd)=}')
print(f'{ru.lapsed_gaussian_detection_function(x, mean, sd, lapse_probability)=}')
print(f'{ru.lapsed_gaussian_detection_function(x, mean, sd, lapse_probability)=}')
print(f'{ru.based_gaussian_detection_function(x, base, mean, sd)=}')
print(f'{ru.based_gaussian_detection_function(x, base, mean, sd)=}')
print(f'{ru.based_gaussian_detection_function(base, base, mean, sd)=}')
print(f'{ru.based_gaussian_detection_function(1.0, base, mean, sd)=}')
print(f'{ru.capped_gaussian_detection_function(x, cap, mean, sd)=}')
print(f'{ru.capped_gaussian_detection_function(x, cap, mean, sd)=}')
print(f'{ru.capped_gaussian_detection_function(0.0, cap, mean, sd)=}')

print('--Symbol Utilities--')
sym_list = [Symbol("one"), Symbol("two"), Symbol("three"), Symbol("four")]
print(f'{su.get_nth_Symbol(sym_list, 2)=}')

print('--Syllable Counter--')
print(f'{count_total_syllables("Memories slip away")=}')

print('--Standard [Utility] Symbols')
print(f'{Default_c=}')
print(f'{Bar_c=}')
print(f'{Pitch_c=}')
print(f'{Nil_c=}')
print(f'{type(Default_c)=}')

print('--Stats--')
data = stats.Mean_accumulator()
for i in range(10):
    data.update(i)
print(f'{data.get_n()=}')
print(f'{data.get_mean()=}')
print(f'{data.get_total()=}')
print(f'{data.get_sample_var()=}')
print(f'{data.get_half_95_ci()=}')

print('--rms stats--')
data = rms.Accumulate_rms_error()
for i in range(20):
    data.update(Point(random.randint(10, 50), random.randint(10, 50)),
                Point(random.randint(10, 50), random.randint(10, 50)))
print(f'{data.get_n()=}')
print(f'{data.get_rms()=}')
data.reset()
print(f'{data.get_n()=}')

print('--exceptions--')
try:
    raise_epic_exception("This is my Epic_exception!")
    print('OOPS, Epic_exception was not successfully raised!')
except Exception as e:
    print(f'Epic_exception was successfully raised!: "{e}"')

try:
    raise_device_exception("This is my Device_exception!")
    print('OOPS, Device_excepton was not successfully raised!')
except Exception as e:
    print(f'Device_exception was successfully raised!: "{e}"')

print('--Coordinator--')
COORDINATOR = Coordinator.get_instance()
# try:
#     COORDINATOR.initialize()
# except Exception as e:
#     print(f'As Expected, this premature initialization raised an error: "{e}"')
print('>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>')
# x = input('enter something here!')
# print('.....')
# print(f'{x=}')

Debug_out('@@@ This Should Not Print Anything BC It Has no sinks!\n')

Exception_out('~~DONE~~\n')

for ot in (Normal_out, Trace_out, PPS_out, Exception_out, Debug_out):
    ot.py_flush()
    ot.clear_py_streams()
    ot.py_close()


sys.exit()
