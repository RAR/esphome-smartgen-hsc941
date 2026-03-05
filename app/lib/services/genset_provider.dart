/// State management provider for the genset connection and polling.
library;

import 'dart:async';
import 'package:flutter/foundation.dart';
import 'api_client.dart';
import '../models/genset_status.dart';
import '../models/config_models.dart';

class GensetProvider extends ChangeNotifier {
  GensetApiClient? _api;
  Timer? _pollTimer;
  GensetStatus? _status;
  FuelStatus? _fuel;
  String? _error;
  bool _connected = false;
  String _host = '';
  int _port = 80;
  int _consecutiveErrors = 0;

  GensetStatus? get status => _status;
  FuelStatus? get fuel => _fuel;
  String? get error => _error;
  bool get connected => _connected;
  String get host => _host;
  int get port => _port;
  GensetApiClient? get api => _api;

  void connect(String host, {int port = 80}) {
    _host = host;
    _port = port;
    _api?.dispose();
    _api = GensetApiClient(host: host, port: port);
    _error = null;
    _consecutiveErrors = 0;
    _connected = false;
    notifyListeners();
    _startPolling();
  }

  void disconnect() {
    _pollTimer?.cancel();
    _pollTimer = null;
    _api?.dispose();
    _api = null;
    _status = null;
    _fuel = null;
    _connected = false;
    _host = '';
    notifyListeners();
  }

  void _startPolling() {
    _pollTimer?.cancel();
    // First poll immediately
    _poll();
    _pollTimer = Timer.periodic(const Duration(milliseconds: 2500), (_) {
      _poll();
    });
  }

  Future<void> _poll() async {
    if (_api == null) return;
    try {
      _status = await _api!.getStatus();
      _error = null;
      _connected = true;
      _consecutiveErrors = 0;
      notifyListeners();
    } catch (e) {
      _consecutiveErrors++;
      if (_consecutiveErrors >= 3) {
        _connected = false;
        _error = 'Connection lost';
      }
      notifyListeners();
    }
  }

  Future<void> refreshFuel() async {
    if (_api == null) return;
    try {
      _fuel = await _api!.getFuel();
      notifyListeners();
    } catch (_) {}
  }

  Future<bool> sendCommand(int coil) async {
    if (_api == null) return false;
    try {
      return await _api!.sendCommand(coil);
    } catch (e) {
      _error = e.toString();
      notifyListeners();
      return false;
    }
  }

  @override
  void dispose() {
    _pollTimer?.cancel();
    _api?.dispose();
    super.dispose();
  }
}
