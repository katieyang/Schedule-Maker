#include "ScheduleMaker_GUI.h"

using namespace System::Windows::Forms;

int main() {
	ScheduleMaker_GUI::ScheduleMaker_GUI^ g = gcnew ScheduleMaker_GUI::ScheduleMaker_GUI();
	Application::Run(g);
	Schedule abc = Schedule();
	return 0;
}