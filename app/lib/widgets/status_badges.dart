/// Status badge chips for mode, alarm, connectivity.
library;

import 'package:flutter/material.dart';
import '../theme/app_theme.dart';

class StatusBadge extends StatelessWidget {
  final String label;
  final Color color;
  final IconData? icon;
  final bool pulse;

  const StatusBadge({
    super.key,
    required this.label,
    required this.color,
    this.icon,
    this.pulse = false,
  });

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 5),
      decoration: BoxDecoration(
        color: color.withAlpha(25),
        borderRadius: BorderRadius.circular(16),
        border: Border.all(color: color.withAlpha(100)),
      ),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          if (icon != null) ...[
            Icon(icon, size: 14, color: color),
            const SizedBox(width: 4),
          ],
          Text(
            label,
            style: TextStyle(
              fontSize: 11,
              fontWeight: FontWeight.w700,
              color: color,
              letterSpacing: 0.5,
            ),
          ),
        ],
      ),
    );
  }
}

class ModeBadge extends StatelessWidget {
  final String mode;

  const ModeBadge({super.key, required this.mode});

  @override
  Widget build(BuildContext context) {
    Color color;
    IconData icon;
    switch (mode) {
      case 'AUTO':
        color = AppTheme.green;
        icon = Icons.autorenew;
        break;
      case 'MANUAL':
        color = AppTheme.orange;
        icon = Icons.pan_tool;
        break;
      case 'STOP':
        color = AppTheme.red;
        icon = Icons.stop_circle_outlined;
        break;
      default:
        color = AppTheme.dim;
        icon = Icons.help_outline;
    }
    return StatusBadge(label: mode, color: color, icon: icon);
  }
}

class ConnectionBadge extends StatelessWidget {
  final bool connected;

  const ConnectionBadge({super.key, required this.connected});

  @override
  Widget build(BuildContext context) {
    return StatusBadge(
      label: connected ? 'LINKED' : 'NO LINK',
      color: connected ? AppTheme.green : AppTheme.red,
      icon: connected ? Icons.link : Icons.link_off,
    );
  }
}

class AlarmBadge extends StatelessWidget {
  final bool isShutdown;
  final bool isAlarm;
  final bool isWarning;

  const AlarmBadge({
    super.key,
    this.isShutdown = false,
    this.isAlarm = false,
    this.isWarning = false,
  });

  @override
  Widget build(BuildContext context) {
    if (isShutdown) {
      return const StatusBadge(
        label: 'SHUTDOWN',
        color: AppTheme.red,
        icon: Icons.error,
      );
    }
    if (isAlarm) {
      return const StatusBadge(
        label: 'ALARM',
        color: AppTheme.red,
        icon: Icons.warning_amber,
      );
    }
    if (isWarning) {
      return const StatusBadge(
        label: 'WARNING',
        color: AppTheme.orange,
        icon: Icons.warning_amber,
      );
    }
    return const SizedBox.shrink();
  }
}
