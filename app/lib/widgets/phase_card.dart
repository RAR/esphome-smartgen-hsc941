/// Phase card — shows voltage, current, power for one AC phase.
library;

import 'package:flutter/material.dart';
import '../theme/app_theme.dart';

class PhaseCard extends StatelessWidget {
  final String phase;
  final double? voltage;
  final double? current;
  final double? power;
  final bool highlight;

  const PhaseCard({
    super.key,
    required this.phase,
    this.voltage,
    this.current,
    this.power,
    this.highlight = false,
  });

  @override
  Widget build(BuildContext context) {
    return Card(
      color: highlight ? AppTheme.card : AppTheme.surface,
      child: Padding(
        padding: const EdgeInsets.symmetric(horizontal: 14, vertical: 12),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Row(
              children: [
                Container(
                  width: 26,
                  height: 26,
                  decoration: BoxDecoration(
                    color: _phaseColor.withAlpha(30),
                    borderRadius: BorderRadius.circular(6),
                  ),
                  alignment: Alignment.center,
                  child: Text(
                    phase,
                    style: TextStyle(
                      fontWeight: FontWeight.w700,
                      fontSize: 13,
                      color: _phaseColor,
                    ),
                  ),
                ),
                const Spacer(),
                if (highlight)
                  Container(
                    width: 6,
                    height: 6,
                    decoration: BoxDecoration(
                      color: AppTheme.green,
                      shape: BoxShape.circle,
                    ),
                  ),
              ],
            ),
            const SizedBox(height: 10),
            _PhaseRow(label: 'Voltage', value: voltage, unit: 'V', decimals: 1),
            const SizedBox(height: 4),
            _PhaseRow(label: 'Current', value: current, unit: 'A', decimals: 1),
            const SizedBox(height: 4),
            _PhaseRow(label: 'Power', value: power, unit: 'kW', decimals: 2),
          ],
        ),
      ),
    );
  }

  Color get _phaseColor {
    switch (phase) {
      case 'A':
        return AppTheme.accent;
      case 'B':
        return AppTheme.green;
      case 'C':
        return AppTheme.orange;
      default:
        return AppTheme.dim;
    }
  }
}

class _PhaseRow extends StatelessWidget {
  final String label;
  final double? value;
  final String unit;
  final int decimals;

  const _PhaseRow({
    required this.label,
    required this.value,
    required this.unit,
    this.decimals = 1,
  });

  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Text(
          label,
          style: const TextStyle(fontSize: 11, color: AppTheme.dim),
        ),
        const Spacer(),
        Text(
          value != null ? '${value!.toStringAsFixed(decimals)} $unit' : '—',
          style: const TextStyle(
            fontSize: 13,
            fontWeight: FontWeight.w600,
            fontFamily: 'monospace',
          ),
        ),
      ],
    );
  }
}
