#include "MainForm.h"
#include <sqlite3.h>


using namespace System;
using namespace System::Windows::Forms;
using namespace SalaryDepartmentApp;

[STAThreadAttribute]
int main(array<String^>^ args) {
    Application::SetCompatibleTextRenderingDefault(false);
    Application::EnableVisualStyles();

    MainForm^ form = gcnew MainForm();
    Application::Run(form);

    return 0;
}