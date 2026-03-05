/// Engine command buttons with confirmation dialogs.
library;

import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../services/genset_provider.dart';
import '../theme/app_theme.dart';

class CommandPanel extends StatelessWidget {
  const CommandPanel({super.key});

  @override
  Widget build(BuildContext context) {
    return Card(
      child: Padding(
        padding: const EdgeInsets.all(14),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Row(
              children: [
                const Icon(Icons.gamepad_outlined,
                    size: 16, color: AppTheme.dim),
                const SizedBox(width: 6),
                Text('Controls',
                    style: Theme.of(context).textTheme.titleMedium),
              ],
            ),
            const SizedBox(height: 12),
            // Row 1: Mode buttons
            Row(
              children: [
                Expanded(
                    child: _CmdButton(
                        label: 'AUTO',
                        icon: Icons.autorenew,
                        color: AppTheme.green,
                        coil: 3)),
                const SizedBox(width: 8),
                Expanded(
                    child: _CmdButton(
                        label: 'MANUAL',
                        icon: Icons.pan_tool_outlined,
                        color: AppTheme.orange,
                        coil: 4)),
              ],
            ),
            const SizedBox(height: 8),
            // Row 2: Engine start/stop
            Row(
              children: [
                Expanded(
                    child: _CmdButton(
                        label: 'START',
                        icon: Icons.play_arrow,
                        color: AppTheme.green,
                        coil: 0,
                        confirm: true)),
                const SizedBox(width: 8),
                Expanded(
                    child: _CmdButton(
                        label: 'STOP',
                        icon: Icons.stop,
                        color: AppTheme.red,
                        coil: 1,
                        confirm: true)),
              ],
            ),
            const SizedBox(height: 8),
            // Row 3: Transfer switch + Reset
            Row(
              children: [
                Expanded(
                    child: _CmdButton(
                        label: 'XFER ON',
                        icon: Icons.power,
                        color: AppTheme.cyan,
                        coil: 6)),
                const SizedBox(width: 8),
                Expanded(
                    child: _CmdButton(
                        label: 'XFER OFF',
                        icon: Icons.power_off,
                        color: AppTheme.dim,
                        coil: 5)),
              ],
            ),
            const SizedBox(height: 8),
            SizedBox(
              width: double.infinity,
              child: _CmdButton(
                  label: 'FAULT RESET',
                  icon: Icons.restart_alt,
                  color: AppTheme.orange,
                  coil: 7),
            ),
          ],
        ),
      ),
    );
  }
}

class _CmdButton extends StatefulWidget {
  final String label;
  final IconData icon;
  final Color color;
  final int coil;
  final bool confirm;

  const _CmdButton({
    required this.label,
    required this.icon,
    required this.color,
    required this.coil,
    this.confirm = false,
  });

  @override
  State<_CmdButton> createState() => _CmdButtonState();
}

class _CmdButtonState extends State<_CmdButton> {
  bool _sending = false;

  Future<void> _execute() async {
    if (widget.confirm) {
      final confirmed = await showDialog<bool>(
        context: context,
        builder: (ctx) => AlertDialog(
          title: Text('Confirm ${widget.label}'),
          content: Text(
              'Are you sure you want to send ${widget.label} command to the generator?'),
          actions: [
            TextButton(
                onPressed: () => Navigator.pop(ctx, false),
                child: const Text('Cancel')),
            ElevatedButton(
              onPressed: () => Navigator.pop(ctx, true),
              style: ElevatedButton.styleFrom(backgroundColor: widget.color),
              child: Text(widget.label),
            ),
          ],
        ),
      );
      if (confirmed != true) return;
    }

    setState(() => _sending = true);
    final ok = await context.read<GensetProvider>().sendCommand(widget.coil);
    if (mounted) {
      setState(() => _sending = false);
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
          content: Text(ok
              ? '${widget.label} sent'
              : '${widget.label} failed'),
          duration: const Duration(seconds: 2),
        ),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    return OutlinedButton.icon(
      onPressed: _sending ? null : _execute,
      style: OutlinedButton.styleFrom(
        foregroundColor: widget.color,
        side: BorderSide(color: widget.color.withAlpha(80)),
        padding: const EdgeInsets.symmetric(vertical: 10),
      ),
      icon: _sending
          ? SizedBox(
              width: 14,
              height: 14,
              child: CircularProgressIndicator(
                  strokeWidth: 2, color: widget.color))
          : Icon(widget.icon, size: 16),
      label: Text(widget.label,
          style: const TextStyle(fontSize: 11, fontWeight: FontWeight.w700)),
    );
  }
}

/// Buzzer silence button — shown inline when alarm/warning active.
class BuzzerButton extends StatefulWidget {
  const BuzzerButton({super.key});

  @override
  State<BuzzerButton> createState() => _BuzzerButtonState();
}

class _BuzzerButtonState extends State<BuzzerButton> {
  bool _sending = false;

  Future<void> _silence() async {
    setState(() => _sending = true);
    final api = context.read<GensetProvider>().api;
    final ok = await api?.silenceBuzzer() ?? false;
    if (mounted) {
      setState(() => _sending = false);
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
            content:
                Text(ok ? 'Buzzer silenced' : 'Failed to silence buzzer')),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    return IconButton.filled(
      onPressed: _sending ? null : _silence,
      icon: _sending
          ? const SizedBox(
              width: 16,
              height: 16,
              child: CircularProgressIndicator(strokeWidth: 2))
          : const Icon(Icons.volume_off, size: 18),
      tooltip: 'Silence buzzer',
      style: IconButton.styleFrom(
        backgroundColor: AppTheme.red.withAlpha(30),
        foregroundColor: AppTheme.red,
      ),
    );
  }
}
