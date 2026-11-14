"""
Pytest test suite for epiclibcpp API.

Tests core functionality of the epiclibcpp package to verify
the pybind11 bindings work correctly on each platform.
"""

import math
import random
import sys

import pytest


class TestSymbolAndPoint:
    """Tests for Symbol and Point classes."""

    def test_symbol_string_value(self):
        from epiclibcpp.epiclib import Symbol

        sym1 = Symbol("sym1")
        sym2 = Symbol("32", True)

        assert sym1.has_string_value() is True
        assert sym2.has_string_value() is True
        assert str(sym1) == "sym1"

    def test_symbol_from_point(self):
        from epiclibcpp.epiclib import Symbol
        from epiclibcpp.epiclib.geometric_utilities import Point

        p = Point(32, 44)
        sym = Symbol(p)

        retrieved = sym.get_point()
        assert retrieved.x == pytest.approx(32.0)
        assert retrieved.y == pytest.approx(44.0)

    def test_point_creation(self):
        from epiclibcpp.epiclib.geometric_utilities import Point

        p = Point(402, 1123)
        assert p.x == pytest.approx(402.0)
        assert p.y == pytest.approx(1123.0)


class TestSpeechWord:
    """Tests for Speech_word class."""

    def test_speech_word_properties(self):
        from epiclibcpp.epiclib import Symbol, Speech_word
        from epiclibcpp.epiclib.geometric_utilities import Point

        sw = Speech_word()
        sw.name = Symbol("my tone")
        sw.location = Point(12.3, 49.7)

        assert sw.location.x == pytest.approx(12.3)
        assert sw.location.y == pytest.approx(49.7)
        # level_left should be accessible
        _ = sw.level_left


class TestDeviceAndModel:
    """Tests for Device_base and Model classes."""

    def test_device_subclass(self):
        from epiclibcpp.epiclib import Device_base
        from epiclibcpp.epiclib.output_tee_globals import Normal_out

        class MyDevice(Device_base):
            def __init__(self, Id, ot):
                super().__init__(Id, ot)

        device = MyDevice("MyTestDevice", Normal_out)
        info = device.processor_info
        assert info is not None

    def test_model_time(self):
        from epiclibcpp.epiclib import Device_base, Model
        from epiclibcpp.epiclib.output_tee_globals import Normal_out

        class MyDevice(Device_base):
            def __init__(self, Id, ot):
                super().__init__(Id, ot)

        device = MyDevice("MyTestDevice", Normal_out)
        model = Model(device)
        time = model.get_time()
        assert isinstance(time, (int, float))


class TestCoordinator:
    """Tests for Coordinator singleton."""

    def test_coordinator_time(self):
        from epiclibcpp.epiclib import Coordinator

        time = Coordinator.get_time()
        assert isinstance(time, (int, float))

    def test_coordinator_instance(self):
        from epiclibcpp.epiclib import Coordinator

        coordinator = Coordinator.get_instance()
        assert coordinator is not None


class TestGeometricUtilities:
    """Tests for geometric utility classes."""

    def test_to_radians(self):
        from epiclibcpp.epiclib import geometric_utilities as gu

        radians = gu.to_radians(180.0)
        assert radians == pytest.approx(math.pi, rel=1e-5)

    def test_cartesian_vector(self):
        from epiclibcpp.epiclib import geometric_utilities as gu
        from epiclibcpp.epiclib.geometric_utilities import Point

        cv1 = gu.Cartesian_vector()
        cv2 = gu.Cartesian_vector(Point(2.3, 2.3), Point(19.08, 20.77))

        assert cv2.delta_x == pytest.approx(16.78, rel=1e-3)

    def test_polar_vector(self):
        from epiclibcpp.epiclib import geometric_utilities as gu
        from epiclibcpp.epiclib.geometric_utilities import Point

        pv1 = gu.Polar_vector()
        pv2 = gu.Polar_vector(Point(2.3, 2.3), Point(19.08, 20.77))

        # theta should be defined
        _ = pv2.theta
        _ = pv2.r

    def test_polar_from_cartesian(self):
        from epiclibcpp.epiclib import geometric_utilities as gu
        from epiclibcpp.epiclib.geometric_utilities import Point

        cv = gu.Cartesian_vector(Point(0, 0), Point(10, 10))
        pv = gu.Polar_vector(cv)

        assert pv.theta == pytest.approx(math.pi / 4, rel=1e-5)

    def test_line_segment(self):
        from epiclibcpp.epiclib import geometric_utilities as gu
        from epiclibcpp.epiclib.geometric_utilities import Point

        ls = gu.Line_segment(Point(0, 0), Point(3, 4))
        size = ls.get_size()

        assert size.x == pytest.approx(3.0)
        assert size.y == pytest.approx(4.0)

    def test_polygon(self):
        from epiclibcpp.epiclib import geometric_utilities as gu
        from epiclibcpp.epiclib.geometric_utilities import Point

        # Triangle
        pg = gu.Polygon([Point(0, -2), Point(2, 2), Point(-2, 2)])
        center = pg.get_center()
        vertices = pg.get_vertices()

        assert len(vertices) == 3
        assert center.y == pytest.approx(2 / 3, rel=1e-3)


class TestViewBase:
    """Tests for View_base subclassing."""

    def test_view_subclass(self):
        from epiclibcpp.epiclib import View_base

        class MyView(View_base):
            def __init__(self):
                super().__init__()
                self.received_text = []

            def notify_append_text(self, text: str):
                self.received_text.append(text)

        view = MyView()
        assert view is not None


class TestEncoders:
    """Tests for encoder base classes."""

    def test_visual_encoder(self):
        from epiclibcpp.epiclib import Visual_encoder_base

        class MyVisEncoder(Visual_encoder_base):
            def __init__(self, Id):
                super().__init__(Id)

        encoder = MyVisEncoder("TestEncoder")
        assert encoder is not None

    def test_auditory_encoder(self):
        from epiclibcpp.epiclib import Auditory_encoder_base

        class MyAudEncoder(Auditory_encoder_base):
            def __init__(self, Id):
                super().__init__(Id)

        encoder = MyAudEncoder("TestEncoder")
        assert encoder is not None


class TestNumericUtilities:
    """Tests for numeric utility functions."""

    def test_round_to_integer(self):
        from epiclibcpp.epiclib import numeric_utilities as nu

        assert nu.round_to_integer(3.7) == 4
        assert nu.round_to_integer(3.2) == 3

    def test_int_to_string(self):
        from epiclibcpp.epiclib import numeric_utilities as nu

        assert nu.int_to_string(123) == "123"

    def test_time_convert(self):
        from epiclibcpp.epiclib import numeric_utilities as nu

        # 1 hour, 2 minutes, 3 seconds = 3723 seconds = 3723000 ms
        result = nu.time_convert(1, 2, 3)
        assert result == 3723000

    def test_logb2(self):
        from epiclibcpp.epiclib import numeric_utilities as nu

        assert nu.logb2(8) == pytest.approx(3.0)
        assert nu.logb2(16) == pytest.approx(4.0)

    def test_pitch_to_semitones(self):
        from epiclibcpp.epiclib import numeric_utilities as nu

        # A4 = 440 Hz = 69 semitones in MIDI, but this function uses a different reference
        result = nu.pitch_to_semitones(440)
        assert isinstance(result, float)


class TestRandomUtilities:
    """Tests for random utility functions."""

    def test_uniform_detection_function(self):
        from epiclibcpp.epiclib import random_utilities as ru

        # Should return True or False based on probability
        result = ru.uniform_detection_function(0.5)
        assert isinstance(result, bool)

    def test_gaussian_detection_function(self):
        from epiclibcpp.epiclib import random_utilities as ru

        result = ru.gaussian_detection_function(0.5, 0.0, 1.0)
        assert isinstance(result, bool)

    def test_lapsed_gaussian_detection_function(self):
        from epiclibcpp.epiclib import random_utilities as ru

        result = ru.lapsed_gaussian_detection_function(0.5, 0.0, 1.0, 0.1)
        assert isinstance(result, bool)

    def test_based_gaussian_detection_function(self):
        from epiclibcpp.epiclib import random_utilities as ru

        result = ru.based_gaussian_detection_function(0.5, 0.1, 0.0, 1.0)
        assert isinstance(result, bool)

    def test_capped_gaussian_detection_function(self):
        from epiclibcpp.epiclib import random_utilities as ru

        result = ru.capped_gaussian_detection_function(0.5, 0.9, 0.0, 1.0)
        assert isinstance(result, bool)


class TestSymbolUtilities:
    """Tests for symbol utility functions."""

    def test_get_nth_symbol(self):
        from epiclibcpp.epiclib import Symbol
        from epiclibcpp.epiclib import symbol_utilities as su

        sym_list = [Symbol("one"), Symbol("two"), Symbol("three"), Symbol("four")]
        result = su.get_nth_Symbol(sym_list, 2)

        assert str(result) == "three"


class TestSyllableCounter:
    """Tests for syllable counting."""

    def test_count_syllables(self):
        from epiclibcpp.epiclib.syllable_counter import count_total_syllables

        count = count_total_syllables("Memories slip away")
        assert count > 0
        assert isinstance(count, int)


class TestStandardSymbols:
    """Tests for standard symbol constants."""

    def test_standard_symbols_exist(self):
        from epiclibcpp.epiclib import Symbol
        from epiclibcpp.epiclib.standard_symbols import Default_c, Nil_c

        assert isinstance(Default_c, Symbol)
        assert isinstance(Nil_c, Symbol)

    def test_standard_utility_symbols_exist(self):
        from epiclibcpp.epiclib import Symbol
        from epiclibcpp.epiclib.standard_utility_symbols import Bar_c, Pitch_c

        assert isinstance(Bar_c, Symbol)
        assert isinstance(Pitch_c, Symbol)


class TestStatistics:
    """Tests for statistics accumulators."""

    def test_mean_accumulator(self):
        from epiclibcpp.epiclib import statistics as stats

        acc = stats.Mean_accumulator()
        for i in range(10):
            acc.update(float(i))

        assert acc.get_n() == 10
        assert acc.get_mean() == pytest.approx(4.5)
        assert acc.get_total() == pytest.approx(45.0)
        assert acc.get_sample_var() > 0
        assert acc.get_half_95_ci() > 0


class TestRmsStatistics:
    """Tests for RMS error statistics."""

    def test_rms_accumulator(self):
        from epiclibcpp.epiclib import rms_statistics as rms
        from epiclibcpp.epiclib.geometric_utilities import Point

        acc = rms.Accumulate_rms_error()
        for _ in range(20):
            acc.update(
                Point(random.randint(10, 50), random.randint(10, 50)),
                Point(random.randint(10, 50), random.randint(10, 50)),
            )

        assert acc.get_n() == 20
        assert acc.get_rms() >= 0

        acc.reset()
        assert acc.get_n() == 0


class TestExceptions:
    """Tests for custom exception raising."""

    def test_epic_exception(self):
        from epiclibcpp.epiclib import raise_epic_exception

        with pytest.raises(Exception) as exc_info:
            raise_epic_exception("Test Epic exception")

        assert "Test Epic exception" in str(exc_info.value)

    def test_device_exception(self):
        from epiclibcpp.epiclib import raise_device_exception

        with pytest.raises(Exception) as exc_info:
            raise_device_exception("Test Device exception")

        assert "Test Device exception" in str(exc_info.value)


class TestOutputTee:
    """Tests for output tee functionality."""

    def test_output_tee_globals_exist(self):
        from epiclibcpp.epiclib.output_tee_globals import (
            Debug_out,
            Exception_out,
            Normal_out,
            Trace_out,
        )
        from epiclibcpp.epiclib.pps_globals import PPS_out

        # All output tees should be accessible
        assert Normal_out is not None
        assert Trace_out is not None
        assert Debug_out is not None
        assert Exception_out is not None
        assert PPS_out is not None

    def test_output_tee_write(self):
        from epiclibcpp.epiclib.output_tee_globals import Normal_out

        # Should not raise
        Normal_out("Test message\n")

    def test_output_tee_bool(self):
        from epiclibcpp.epiclib.output_tee_globals import Normal_out

        # Should return bool indicating if output is enabled
        result = bool(Normal_out)
        assert isinstance(result, bool)
