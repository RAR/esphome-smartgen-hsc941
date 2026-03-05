/// Dashboard / Monitor screen — main real-time view of the generator.
library;

import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../services/genset_provider.dart';
import '../models/genset_status.dart';
import '../theme/app_theme.dart';
import '../widgets/radial_gauge.dart';
import '../widgets/status_badges.dart';
import '../widgets/phase_card.dart';
import '../widgets/command_panel.dart';

class DashboardScreen extends StatefulWidget {
  const DashboardScreen({super.key});

  @override
  State<DashboardScreen> createState() => _DashboardScreenState();
}

class _DashboardScreenState extends State<DashboardScreen> {
  @override
  void initState() {
    super.initState();
    // Kick off a fuel refresh on first load
    Future.microtask(
        () => context.read<GensetProvider>().refreshFuel());
  }

  @override
  Widget build(BuildContext context) {
    return Consumer<GensetProvider>(
      builder: (context, provider, _) {
        final status = provider.status;
        if (status == null) {
          return const Center(child: CircularProgressIndicator());
        }
        return RefreshIndicator(
          onRefresh: () async => provider.refreshFuel(),
          child: ListView(
            padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 8),
            children: [
              _buildHeader(context, status, provider),
              const SizedBox(height: 12),
              _buildGaugeRow(status),
              const SizedBox(height: 12),
              _buildPhaseSection(status),
              const SizedBox(height: 12),
              _buildEngineCard(status),
              const SizedBox(height: 12),
              _buildFuelCard(context, provider),
              const SizedBox(height: 12),
              _buildRelayCard(status, provider),
              const SizedBox(height: 12),
              const CommandPanel(),
              const SizedBox(height: 20),
            ],
          ),
        );
      },
    );
  }

  Widget _buildHeader(
      BuildContext context, GensetStatus status, GensetProvider provider) {
    return Row(
      children: [
        ModeBadge(mode: status.modeString),
        const SizedBox(width: 8),
        ConnectionBadge(connected: provider.connected),
        if (status.isAlarm || status.isWarning || status.isShutdown) ...[
          const SizedBox(width: 8),
          AlarmBadge(
            isShutdown: status.isShutdown,
            isAlarm: status.isAlarm,
            isWarning: status.isWarning,
          ),
          const SizedBox(width: 4),
          const BuzzerButton(),
        ],
        const Spacer(),
        if (status.mainsOk != null)
          StatusBadge(
            label: status.mainsOk! ? 'MAINS OK' : 'MAINS FAIL',
            color: status.mainsOk! ? AppTheme.green : AppTheme.red,
            icon: status.mainsOk! ? Icons.electrical_services : Icons.flash_off,
          ),
      ],
    );
  }

  Widget _buildGaugeRow(GensetStatus status) {
    final isSingle = status.singlePhase ?? true;
    return SizedBox(
      height: 170,
      child: Row(
        children: [
          Expanded(
            child: RadialGauge(
              label: isSingle ? 'Voltage' : 'Avg Voltage',
              value: status.genVoltageA ?? 0,
              unit: 'V',
              min: 0,
              max: 300,
              color: AppTheme.accent,
              decimals: 1,
            ),
          ),
          Expanded(
            child: RadialGauge(
              label: 'Frequency',
              value: status.genFrequency ?? 0,
              unit: 'Hz',
              min: 0,
              max: 80,
              color: AppTheme.cyan,
              decimals: 1,
            ),
          ),
          Expanded(
            child: RadialGauge(
              label: 'RPM',
              value: status.engineSpeed ?? 0,
              unit: 'rpm',
              min: 0,
              max: 4000,
              color: AppTheme.green,
              decimals: 0,
            ),
          ),
          Expanded(
            child: RadialGauge(
              label: 'Load',
              value: status.outputLoad ?? 0,
              unit: '%',
              min: 0,
              max: 100,
              color: _loadColor(status.outputLoad ?? 0),
              decimals: 0,
            ),
          ),
        ],
      ),
    );
  }

  Color _loadColor(double load) {
    if (load > 90) return AppTheme.red;
    if (load > 70) return AppTheme.orange;
    return AppTheme.green;
  }

  Widget _buildPhaseSection(GensetStatus status) {
    final isSingle = status.singlePhase ?? true;
    if (isSingle) {
      return PhaseCard(
        phase: 'A',
        voltage: status.genVoltageA,
        current: status.phaseACurrent,
        power: status.phaseAPower,
        highlight: status.isGenOnLoad,
      );
    }
    return Row(
      children: [
        Expanded(
          child: PhaseCard(
            phase: 'A',
            voltage: status.genVoltageA,
            current: status.phaseACurrent,
            power: status.phaseAPower,
            highlight: status.isGenOnLoad,
          ),
        ),
        const SizedBox(width: 8),
        Expanded(
          child: PhaseCard(
            phase: 'B',
            voltage: status.genVoltageB,
            current: status.phaseBCurrent,
            power: status.phaseBPower,
            highlight: status.isGenOnLoad,
          ),
        ),
        const SizedBox(width: 8),
        Expanded(
          child: PhaseCard(
            phase: 'C',
            voltage: status.genVoltageC,
            current: status.phaseCCurrent,
            power: status.phaseCPower,
            highlight: status.isGenOnLoad,
          ),
        ),
      ],
    );
  }

  Widget _buildEngineCard(GensetStatus status) {
    return Card(
      child: Padding(
        padding: const EdgeInsets.all(14),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Row(
              children: [
                const Icon(Icons.engineering, size: 16, color: AppTheme.dim),
                const SizedBox(width: 6),
                Text('Engine',
                    style: Theme.of(context).textTheme.titleMedium),
                const Spacer(),
                StatusBadge(
                  label: status.isEngineRunning ? 'RUNNING' : 'STOPPED',
                  color: status.isEngineRunning
                      ? AppTheme.green
                      : AppTheme.dim,
                  icon: status.isEngineRunning
                      ? Icons.local_fire_department
                      : Icons.pause_circle_outline,
                ),
              ],
            ),
            const SizedBox(height: 12),
            Row(
              children: [
                Expanded(
                    child: _InfoTile(
                        icon: Icons.thermostat,
                        label: 'Water Temp',
                        value: status.waterTemp,
                        unit: '°C',
                        warn: (status.waterTemp ?? 0) > 95)),
                Expanded(
                    child: _InfoTile(
                        icon: Icons.speed,
                        label: 'Oil Pressure',
                        value: status.oilPressure,
                        unit: 'kPa')),
              ],
            ),
            const SizedBox(height: 8),
            Row(
              children: [
                Expanded(
                    child: _InfoTile(
                        icon: Icons.battery_charging_full,
                        label: 'Battery',
                        value: status.batteryVoltage,
                        unit: 'V',
                        warn: (status.batteryVoltage ?? 13) < 11.5)),
                Expanded(
                    child: _InfoTile(
                        icon: Icons.bolt,
                        label: 'Charge',
                        value: status.chargeVoltage,
                        unit: 'V')),
              ],
            ),
            const Divider(height: 20),
            Row(
              children: [
                Expanded(
                    child: _InfoTile(
                        icon: Icons.timer,
                        label: 'Total Hours',
                        value: status.totalHours,
                        unit: 'h',
                        decimals: 1)),
                Expanded(
                    child: _InfoTile(
                        icon: Icons.replay,
                        label: 'Total Starts',
                        value: status.totalStarts,
                        unit: '',
                        decimals: 0)),
                Expanded(
                    child: _InfoTile(
                        icon: Icons.electric_meter,
                        label: 'Total Energy',
                        value: status.totalEnergy,
                        unit: 'kWh',
                        decimals: 1)),
              ],
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildFuelCard(BuildContext context, GensetProvider provider) {
    final fuel = provider.fuel;
    if (fuel == null) {
      return Card(
        child: Padding(
          padding: const EdgeInsets.all(14),
          child: Row(
            children: [
              const Icon(Icons.local_gas_station,
                  size: 16, color: AppTheme.dim),
              const SizedBox(width: 6),
              Text('Fuel', style: Theme.of(context).textTheme.titleMedium),
              const Spacer(),
              const SizedBox(
                width: 16,
                height: 16,
                child: CircularProgressIndicator(strokeWidth: 2),
              ),
            ],
          ),
        ),
      );
    }
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
                Text('Fuel', style: Theme.of(context).textTheme.titleMedium),
                const Spacer(),
                Text(
                  '${fuel.pct.toStringAsFixed(0)}%',
                  style: TextStyle(
                    fontSize: 18,
                    fontWeight: FontWeight.w700,
                    color: fuel.pct < 20 ? AppTheme.red : AppTheme.text,
                  ),
                ),
              ],
            ),
            const SizedBox(height: 10),
            // Fuel bar
            ClipRRect(
              borderRadius: BorderRadius.circular(4),
              child: LinearProgressIndicator(
                value: (fuel.pct / 100).clamp(0, 1),
                minHeight: 8,
                backgroundColor: AppTheme.faint,
                color: fuel.pct < 20
                    ? AppTheme.red
                    : fuel.pct < 40
                        ? AppTheme.orange
                        : AppTheme.green,
              ),
            ),
            const SizedBox(height: 10),
            Row(
              children: [
                _FuelStat(
                    label: 'Remaining',
                    value:
                        '${fuel.remain.toStringAsFixed(1)} ${fuel.volUnit}'),
                _FuelStat(
                    label: 'Burn Rate',
                    value:
                        '${fuel.effRate.toStringAsFixed(2)} ${fuel.rateUnit}'),
                _FuelStat(
                    label: 'Est. Runtime',
                    value: '${fuel.estHours.toStringAsFixed(1)} h'),
              ],
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildRelayCard(GensetStatus status, GensetProvider provider) {
    if (status.relays.isEmpty) return const SizedBox.shrink();
    return Card(
      child: Padding(
        padding: const EdgeInsets.all(14),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Row(
              children: [
                const Icon(Icons.toggle_on, size: 16, color: AppTheme.dim),
                const SizedBox(width: 6),
                Text('Relays',
                    style: Theme.of(context).textTheme.titleMedium),
              ],
            ),
            const SizedBox(height: 10),
            ...status.relays.map((relay) => Padding(
                  padding: const EdgeInsets.symmetric(vertical: 3),
                  child: Row(
                    children: [
                      Icon(
                        relay.state
                            ? Icons.radio_button_checked
                            : Icons.radio_button_off,
                        size: 14,
                        color: relay.state ? AppTheme.green : AppTheme.dim,
                      ),
                      const SizedBox(width: 8),
                      Expanded(
                          child: Text(relay.name,
                              style: const TextStyle(fontSize: 13))),
                      Switch(
                        value: relay.state,
                        onChanged: (_) async {
                          await provider.api?.toggleRelay(relay.index);
                        },
                        activeTrackColor: AppTheme.green,
                      ),
                    ],
                  ),
                )),
          ],
        ),
      ),
    );
  }
}

/// Small info tile used in the engine card.
class _InfoTile extends StatelessWidget {
  final IconData icon;
  final String label;
  final double? value;
  final String unit;
  final int decimals;
  final bool warn;

  const _InfoTile({
    required this.icon,
    required this.label,
    this.value,
    this.unit = '',
    this.decimals = 1,
    this.warn = false,
  });

  @override
  Widget build(BuildContext context) {
    final color = warn ? AppTheme.red : AppTheme.text;
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        Row(
          mainAxisSize: MainAxisSize.min,
          children: [
            Icon(icon, size: 12, color: AppTheme.dim),
            const SizedBox(width: 4),
            Text(label,
                style: const TextStyle(fontSize: 10, color: AppTheme.dim)),
          ],
        ),
        const SizedBox(height: 2),
        Text(
          value != null
              ? '${value!.toStringAsFixed(decimals)} $unit'
              : '—',
          style: TextStyle(
            fontSize: 15,
            fontWeight: FontWeight.w600,
            fontFamily: 'monospace',
            color: color,
          ),
        ),
      ],
    );
  }
}

class _FuelStat extends StatelessWidget {
  final String label;
  final String value;

  const _FuelStat({required this.label, required this.value});

  @override
  Widget build(BuildContext context) {
    return Expanded(
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Text(label,
              style: const TextStyle(fontSize: 10, color: AppTheme.dim)),
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
