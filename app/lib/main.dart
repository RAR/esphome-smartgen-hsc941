/// Entry point for the GenSet Controller app.
library;

import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'services/genset_provider.dart';
import 'theme/app_theme.dart';
import 'screens/discovery_screen.dart';

void main() {
  WidgetsFlutterBinding.ensureInitialized();
  runApp(const GensetControllerApp());
}

class GensetControllerApp extends StatelessWidget {
  const GensetControllerApp({super.key});

  @override
  Widget build(BuildContext context) {
    return ChangeNotifierProvider(
      create: (_) => GensetProvider(),
      child: MaterialApp(
        title: 'GenSet Controller',
        debugShowCheckedModeBanner: false,
        theme: AppTheme.darkTheme,
        home: const DiscoveryScreen(),
      ),
    );
  }
}
