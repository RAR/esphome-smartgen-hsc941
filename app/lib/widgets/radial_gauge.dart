/// Radial gauge widget for voltage, frequency, RPM, load.
library;

import 'dart:math';
import 'package:flutter/material.dart';
import '../theme/app_theme.dart';

class RadialGauge extends StatelessWidget {
  final String label;
  final String unit;
  final double? value;
  final double min;
  final double max;
  final Color color;
  final int decimals;

  const RadialGauge({
    super.key,
    required this.label,
    required this.unit,
    this.value,
    required this.min,
    required this.max,
    required this.color,
    this.decimals = 0,
  });

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      width: 120,
      height: 140,
      child: Column(
        children: [
          SizedBox(
            width: 110,
            height: 110,
            child: CustomPaint(
              painter: _GaugePainter(
                value: value,
                min: min,
                max: max,
                color: color,
              ),
              child: Center(
                child: Column(
                  mainAxisSize: MainAxisSize.min,
                  children: [
                    Text(
                      value != null
                          ? value!.toStringAsFixed(decimals)
                          : '--',
                      style: TextStyle(
                        fontSize: 20,
                        fontWeight: FontWeight.w700,
                        color: AppTheme.text,
                        fontFeatures: const [FontFeature.tabularFigures()],
                      ),
                    ),
                    Text(
                      unit,
                      style: TextStyle(fontSize: 11, color: AppTheme.dim),
                    ),
                  ],
                ),
              ),
            ),
          ),
          const SizedBox(height: 2),
          Text(
            label,
            style: TextStyle(
              fontSize: 11,
              fontWeight: FontWeight.w600,
              color: AppTheme.dim,
            ),
          ),
          Text(
            '${min.toInt()} – ${max.toInt()} $unit',
            style: TextStyle(fontSize: 9, color: AppTheme.faint),
          ),
        ],
      ),
    );
  }
}

class _GaugePainter extends CustomPainter {
  final double? value;
  final double min;
  final double max;
  final Color color;

  _GaugePainter({
    required this.value,
    required this.min,
    required this.max,
    required this.color,
  });

  @override
  void paint(Canvas canvas, Size size) {
    final center = Offset(size.width / 2, size.height / 2);
    final radius = size.width / 2 - 8;
    const startAngle = 135.0;
    const sweepAngle = 270.0;

    // Track
    final trackPaint = Paint()
      ..color = AppTheme.border
      ..style = PaintingStyle.stroke
      ..strokeWidth = 8
      ..strokeCap = StrokeCap.round;

    canvas.drawArc(
      Rect.fromCircle(center: center, radius: radius),
      _degToRad(startAngle),
      _degToRad(sweepAngle),
      false,
      trackPaint,
    );

    // Fill
    if (value != null) {
      final pct = ((value! - min) / (max - min)).clamp(0.0, 1.0);
      final fillPaint = Paint()
        ..color = color
        ..style = PaintingStyle.stroke
        ..strokeWidth = 8
        ..strokeCap = StrokeCap.round;

      canvas.drawArc(
        Rect.fromCircle(center: center, radius: radius),
        _degToRad(startAngle),
        _degToRad(sweepAngle * pct),
        false,
        fillPaint,
      );
    }
  }

  double _degToRad(double deg) => deg * pi / 180;

  @override
  bool shouldRepaint(covariant _GaugePainter old) =>
      old.value != value || old.color != color;
}
