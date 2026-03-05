/// Config screen — exercise schedule, fuel info, maintenance tracking.
library;

import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../services/genset_provider.dart';
import '../models/config_models.dart';
import '../theme/app_theme.dart';

class ConfigScreen extends StatefulWidget {
  const ConfigScreen({super.key});

  @override
  State<ConfigScreen> createState() => _ConfigScreenState();
}

class _ConfigScreenState extends State<ConfigScreen> {
  ExerciseConfig? _exercise;
  FuelStatus? _fuel;
  Map<String, dynamic>? _maintenance;
  bool _loading = true;
  String? _error;

  @override
  void initState() {
    super.initState();
    _loadAll();
  }

  Future<void> _loadAll() async {
    setState(() {
      _loading = true;
      _error = null;
    });
    final api = context.read<GensetProvider>().api;
    if (api == null) return;

    try {
      final results = await Future.wait([
        api.getExercise(),
        api.getFuel(),
        api.getMaintenance(),
      ]);
      if (mounted) {
        setState(() {
          _exercise = results[0] as ExerciseConfig;
          _fuel = results[1] as FuelStatus;
          _maintenance = results[2] as Map<String, dynamic>;
          _loading = false;
        });
      }
    } catch (e) {
      if (mounted) {
        setState(() {
          _error = e.toString();
          _loading = false;
        });
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    if (_loading) {
      return const Center(child: CircularProgressIndicator());
    }
    if (_error != null) {
      return Center(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            Icon(Icons.error_outline, size: 48, color: AppTheme.red),
            const SizedBox(height: 12),
            Text(_error!, style: const TextStyle(color: AppTheme.dim)),
            const SizedBox(height: 12),
            ElevatedButton.icon(
              onPressed: _loadAll,
              icon: const Icon(Icons.refresh, size: 16),
              label: const Text('Retry'),
            ),
          ],
        ),
      );
    }
    return RefreshIndicator(
      onRefresh: _loadAll,
      child: ListView(
        padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 8),
        children: [
          _buildExerciseCard(),
          const SizedBox(height: 12),
          _buildFuelInfoCard(),
          const SizedBox(height: 12),
          _buildMaintenanceCard(),
          const SizedBox(height: 20),
        ],
      ),
    );
  }

  // ── Exercise ──
  Widget _buildExerciseCard() {
    final ex = _exercise;
    if (ex == null) return const SizedBox.shrink();
    final days = [
      'Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday',
      'Saturday'
    ];
    final dayStr = (ex.day >= 0 && ex.day < 7) ? days[ex.day] : 'Day ${ex.day}';
    final timeStr =
        '${ex.hour.toString().padLeft(2, '0')}:${ex.minute.toString().padLeft(2, '0')}';

    return Card(
      child: Padding(
        padding: const EdgeInsets.all(14),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Row(
              children: [
                const Icon(Icons.schedule, size: 16, color: AppTheme.dim),
                const SizedBox(width: 6),
                Text('Exercise Schedule',
                    style: Theme.of(context).textTheme.titleMedium),
                const Spacer(),
                Container(
                  padding:
                      const EdgeInsets.symmetric(horizontal: 8, vertical: 3),
                  decoration: BoxDecoration(
                    color: ex.enabled
                        ? AppTheme.green.withAlpha(25)
                        : AppTheme.faint,
                    borderRadius: BorderRadius.circular(12),
                  ),
                  child: Text(
                    ex.enabled ? 'ENABLED' : 'DISABLED',
                    style: TextStyle(
                      fontSize: 10,
                      fontWeight: FontWeight.w700,
                      color: ex.enabled ? AppTheme.green : AppTheme.dim,
                    ),
                  ),
                ),
              ],
            ),
            const SizedBox(height: 12),
            _ConfigRow(label: 'Day', value: dayStr),
            _ConfigRow(label: 'Time', value: timeStr),
            _ConfigRow(label: 'Duration', value: '${ex.durationMin} min'),
            _ConfigRow(
                label: 'Load Transfer',
                value: ex.loadTransfer ? 'Yes' : 'No'),
            _ConfigRow(label: 'State', value: ex.state),
            _ConfigRow(label: 'Last Run', value: ex.lastRun),
            if (ex.next != null)
              _ConfigRow(label: 'Next', value: ex.next!),
            const SizedBox(height: 10),
            Row(
              children: [
                Expanded(
                  child: OutlinedButton.icon(
                    onPressed: _runExercise,
                    icon: const Icon(Icons.play_arrow, size: 16),
                    label: const Text('Run Now',
                        style: TextStyle(fontSize: 12)),
                    style: OutlinedButton.styleFrom(
                        foregroundColor: AppTheme.green),
                  ),
                ),
                const SizedBox(width: 8),
                Expanded(
                  child: OutlinedButton.icon(
                    onPressed: _stopExercise,
                    icon: const Icon(Icons.stop, size: 16),
                    label:
                        const Text('Stop', style: TextStyle(fontSize: 12)),
                    style: OutlinedButton.styleFrom(
                        foregroundColor: AppTheme.red),
                  ),
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }

  Future<void> _runExercise() async {
    final api = context.read<GensetProvider>().api;
    final ok = await api?.runExercise() ?? false;
    if (mounted) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
            content:
                Text(ok ? 'Exercise started' : 'Failed to start exercise')),
      );
      _loadAll();
    }
  }

  Future<void> _stopExercise() async {
    final api = context.read<GensetProvider>().api;
    final ok = await api?.stopExercise() ?? false;
    if (mounted) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
            content:
                Text(ok ? 'Exercise stopped' : 'Failed to stop exercise')),
      );
      _loadAll();
    }
  }

  // ── Fuel Info ──
  Widget _buildFuelInfoCard() {
    final fuel = _fuel;
    if (fuel == null) return const SizedBox.shrink();
    return Card(
      child: Padding(
        padding: const EdgeInsets.all(14),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Row(
              children: [
                const Icon(Icons.local_gas_station,
                    size: 16, color: AppTheme.dim),
                const SizedBox(width: 6),
                Text('Fuel Configuration',
                    style: Theme.of(context).textTheme.titleMedium),
              ],
            ),
            const SizedBox(height: 12),
            _ConfigRow(label: 'Fuel Type', value: fuel.fuelType),
            _ConfigRow(
                label: 'Tank Size',
                value: '${fuel.tankSize.toStringAsFixed(1)} ${fuel.volUnit}'),
            _ConfigRow(
                label: 'Idle Rate',
                value:
                    '${fuel.rateIdle.toStringAsFixed(2)} ${fuel.rateUnit}'),
            _ConfigRow(
                label: 'Full Rate',
                value:
                    '${fuel.rateFull.toStringAsFixed(2)} ${fuel.rateUnit}'),
            _ConfigRow(
                label: 'Fuel Sensor',
                value: fuel.hasSensor ? 'Connected' : 'Not connected'),
          ],
        ),
      ),
    );
  }

  // ── Maintenance ──
  Widget _buildMaintenanceCard() {
    if (_maintenance == null) return const SizedBox.shrink();
    final items = (_maintenance!['items'] as List?)
            ?.map((e) => MaintenanceItem.fromJson(e as Map<String, dynamic>))
            .toList() ??
        [];
    final totalHours = (_maintenance!['total_hours'] as num?)?.toDouble() ?? 0;

    return Card(
      child: Padding(
        padding: const EdgeInsets.all(14),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Row(
              children: [
                const Icon(Icons.build_outlined,
                    size: 16, color: AppTheme.dim),
                const SizedBox(width: 6),
                Text('Maintenance',
                    style: Theme.of(context).textTheme.titleMedium),
              ],
            ),
            const SizedBox(height: 12),
            if (items.isEmpty)
              const Text('No maintenance items configured',
                  style: TextStyle(color: AppTheme.dim, fontSize: 13)),
            ...items.map((item) {
              final remaining = item.remainingHours(totalHours);
              final pct =
                  (1 - remaining / item.intervalHours).clamp(0.0, 1.0);
              final overdue = remaining <= 0;
              return Padding(
                padding: const EdgeInsets.symmetric(vertical: 6),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Row(
                      children: [
                        Expanded(
                            child: Text(item.name,
                                style: const TextStyle(
                                    fontSize: 13,
                                    fontWeight: FontWeight.w600))),
                        Text(
                          overdue
                              ? 'OVERDUE'
                              : '${remaining.toStringAsFixed(0)} h left',
                          style: TextStyle(
                            fontSize: 11,
                            fontWeight: FontWeight.w600,
                            color: overdue
                                ? AppTheme.red
                                : remaining < 50
                                    ? AppTheme.orange
                                    : AppTheme.dim,
                          ),
                        ),
                      ],
                    ),
                    const SizedBox(height: 4),
                    ClipRRect(
                      borderRadius: BorderRadius.circular(3),
                      child: LinearProgressIndicator(
                        value: pct,
                        minHeight: 4,
                        backgroundColor: AppTheme.faint,
                        color: overdue
                            ? AppTheme.red
                            : remaining < 50
                                ? AppTheme.orange
                                : AppTheme.green,
                      ),
                    ),
                  ],
                ),
              );
            }),
          ],
        ),
      ),
    );
  }
}

class _ConfigRow extends StatelessWidget {
  final String label;
  final String value;

  const _ConfigRow({required this.label, required this.value});

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(vertical: 3),
      child: Row(
        children: [
          Text(label,
              style: const TextStyle(fontSize: 12, color: AppTheme.dim)),
          const Spacer(),
          Text(value,
              style: const TextStyle(
                  fontSize: 13,
                  fontWeight: FontWeight.w600,
                  fontFamily: 'monospace')),
        ],
      ),
    );
  }
}
