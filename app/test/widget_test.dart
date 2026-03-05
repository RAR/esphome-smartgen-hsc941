import 'package:flutter_test/flutter_test.dart';
import 'package:genset_controller/main.dart';

void main() {
  testWidgets('App smoke test — renders discovery screen',
      (WidgetTester tester) async {
    await tester.pumpWidget(const GensetControllerApp());
    expect(find.text('GenSet Controller'), findsOneWidget);
  });
}
