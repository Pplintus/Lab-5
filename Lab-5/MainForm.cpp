#include "MainForm.h"
#include "EditForm.h"

using namespace SalaryDepartmentApp;

MainForm::MainForm() {
    dataManager = gcnew DataManager();
    InitializeComponent();

    // Загружаем тестовые данные
    dataManager->LoadFromFiles("employees.txt", "worktypes.txt");
    RefreshData();
}

void MainForm::InitializeComponent() {
    this->Text = "Отдел расчета зарплаты";
    this->Size = System::Drawing::Size(900, 600);
    this->StartPosition = FormStartPosition::CenterScreen;

    // Панель заголовка
    System::Windows::Forms::Panel^ headerPanel = gcnew System::Windows::Forms::Panel();
    headerPanel->Dock = System::Windows::Forms::DockStyle::Top;
    headerPanel->Height = 70;
    headerPanel->BackColor = System::Drawing::Color::LightBlue;

    System::Windows::Forms::Label^ title = gcnew System::Windows::Forms::Label();
    title->Text = "Отдел расчета зарплаты \"Опушечка\"";
    title->Font = gcnew System::Drawing::Font("Arial", 16, FontStyle::Bold);
    title->Location = System::Drawing::Point(10, 10);
    title->AutoSize = true;

    lblTotalSalary = gcnew System::Windows::Forms::Label();
    lblTotalSalary->Text = "Общая сумма выплат: 0 руб.";
    lblTotalSalary->Font = gcnew System::Drawing::Font("Arial", 10);
    lblTotalSalary->Location = System::Drawing::Point(10, 40);
    lblTotalSalary->AutoSize = true;

    headerPanel->Controls->Add(title);
    headerPanel->Controls->Add(lblTotalSalary);
    this->Controls->Add(headerPanel);

    // Меню
    System::Windows::Forms::MenuStrip^ menu = gcnew System::Windows::Forms::MenuStrip();

    System::Windows::Forms::ToolStripMenuItem^ fileMenu = gcnew System::Windows::Forms::ToolStripMenuItem("Файл");
    System::Windows::Forms::ToolStripMenuItem^ loadItem = gcnew System::Windows::Forms::ToolStripMenuItem("Загрузить");
    System::Windows::Forms::ToolStripMenuItem^ saveItem = gcnew System::Windows::Forms::ToolStripMenuItem("Сохранить");
    System::Windows::Forms::ToolStripMenuItem^ exitItem = gcnew System::Windows::Forms::ToolStripMenuItem("Выход");

    loadItem->Click += gcnew System::EventHandler(this, &MainForm::MenuLoad_Click);
    saveItem->Click += gcnew System::EventHandler(this, &MainForm::MenuSave_Click);
    exitItem->Click += gcnew System::EventHandler(this, &MainForm::MenuExit_Click);

    fileMenu->DropDownItems->Add(loadItem);
    fileMenu->DropDownItems->Add(saveItem);
    // Используем Add вместо AddSeparator
    fileMenu->DropDownItems->Add(gcnew System::Windows::Forms::ToolStripSeparator()); // Исправлено!
    fileMenu->DropDownItems->Add(exitItem);

    System::Windows::Forms::ToolStripMenuItem^ dataMenu = gcnew System::Windows::Forms::ToolStripMenuItem("Данные");
    System::Windows::Forms::ToolStripMenuItem^ addEmpItem = gcnew System::Windows::Forms::ToolStripMenuItem("Добавить сотрудника");
    System::Windows::Forms::ToolStripMenuItem^ addWtItem = gcnew System::Windows::Forms::ToolStripMenuItem("Добавить должность");

    addEmpItem->Click += gcnew System::EventHandler(this, &MainForm::AddEmployee_Click);
    addWtItem->Click += gcnew System::EventHandler(this, &MainForm::AddWorkType_Click);

    dataMenu->DropDownItems->Add(addEmpItem);
    dataMenu->DropDownItems->Add(addWtItem);

    System::Windows::Forms::ToolStripMenuItem^ sortMenu = gcnew System::Windows::Forms::ToolStripMenuItem("Сортировка");
    System::Windows::Forms::ToolStripMenuItem^ sortNameItem = gcnew System::Windows::Forms::ToolStripMenuItem("По имени");
    System::Windows::Forms::ToolStripMenuItem^ sortSalaryItem = gcnew System::Windows::Forms::ToolStripMenuItem("По зарплате");

    sortNameItem->Click += gcnew System::EventHandler(this, &MainForm::SortByName_Click);
    sortSalaryItem->Click += gcnew System::EventHandler(this, &MainForm::SortBySalary_Click);

    sortMenu->DropDownItems->Add(sortNameItem);
    sortMenu->DropDownItems->Add(sortSalaryItem);

    System::Windows::Forms::ToolStripMenuItem^ reportMenu = gcnew System::Windows::Forms::ToolStripMenuItem("Отчет");
    System::Windows::Forms::ToolStripMenuItem^ totalItem = gcnew System::Windows::Forms::ToolStripMenuItem("Общая сумма");

    totalItem->Click += gcnew System::EventHandler(this, &MainForm::ShowTotal_Click);
    reportMenu->DropDownItems->Add(totalItem);

    menu->Items->Add(fileMenu);
    menu->Items->Add(dataMenu);
    menu->Items->Add(sortMenu);
    menu->Items->Add(reportMenu);

    this->MainMenuStrip = menu;
    this->Controls->Add(menu);

    // Табы
    tabControl = gcnew System::Windows::Forms::TabControl();
    tabControl->Dock = System::Windows::Forms::DockStyle::Fill;
    tabControl->Location = System::Drawing::Point(0, 94);

    // Вкладка сотрудников
    System::Windows::Forms::TabPage^ tabEmployees = gcnew System::Windows::Forms::TabPage("Сотрудники");

    dataGridEmployees = gcnew System::Windows::Forms::DataGridView();
    dataGridEmployees->Dock = System::Windows::Forms::DockStyle::Fill;
    dataGridEmployees->AllowUserToAddRows = false;
    dataGridEmployees->ReadOnly = true;
    dataGridEmployees->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;

    // Кнопки для сотрудников
    System::Windows::Forms::FlowLayoutPanel^ empButtons = gcnew System::Windows::Forms::FlowLayoutPanel();
    empButtons->Dock = System::Windows::Forms::DockStyle::Top;
    empButtons->Height = 40;

    System::Windows::Forms::Button^ btnAddEmp = gcnew System::Windows::Forms::Button();
    btnAddEmp->Text = "Добавить";
    btnAddEmp->Click += gcnew System::EventHandler(this, &MainForm::AddEmployee_Click);

    System::Windows::Forms::Button^ btnDelEmp = gcnew System::Windows::Forms::Button();
    btnDelEmp->Text = "Удалить";
    btnDelEmp->Click += gcnew System::EventHandler(this, &MainForm::DeleteEmployee_Click);

    empButtons->Controls->Add(btnAddEmp);
    empButtons->Controls->Add(btnDelEmp);

    tabEmployees->Controls->Add(dataGridEmployees);
    tabEmployees->Controls->Add(empButtons);

    // Вкладка должностей
    System::Windows::Forms::TabPage^ tabWorkTypes = gcnew System::Windows::Forms::TabPage("Должности");

    dataGridWorkTypes = gcnew System::Windows::Forms::DataGridView();
    dataGridWorkTypes->Dock = System::Windows::Forms::DockStyle::Fill;
    dataGridWorkTypes->AllowUserToAddRows = false;
    dataGridWorkTypes->ReadOnly = true;
    dataGridWorkTypes->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;

    // Кнопки для должностей
    System::Windows::Forms::FlowLayoutPanel^ wtButtons = gcnew System::Windows::Forms::FlowLayoutPanel();
    wtButtons->Dock = System::Windows::Forms::DockStyle::Top;
    wtButtons->Height = 40;

    System::Windows::Forms::Button^ btnAddWt = gcnew System::Windows::Forms::Button();
    btnAddWt->Text = "Добавить";
    btnAddWt->Click += gcnew System::EventHandler(this, &MainForm::AddWorkType_Click);

    System::Windows::Forms::Button^ btnDelWt = gcnew System::Windows::Forms::Button();
    btnDelWt->Text = "Удалить";
    btnDelWt->Click += gcnew System::EventHandler(this, &MainForm::DeleteWorkType_Click);

    wtButtons->Controls->Add(btnAddWt);
    wtButtons->Controls->Add(btnDelWt);

    tabWorkTypes->Controls->Add(dataGridWorkTypes);
    tabWorkTypes->Controls->Add(wtButtons);

    tabControl->TabPages->Add(tabEmployees);
    tabControl->TabPages->Add(tabWorkTypes);

    this->Controls->Add(tabControl);
}

void MainForm::RefreshData() {
    // Обновляем общую сумму
    double total = dataManager->CalculateTotalSalary();
    lblTotalSalary->Text = System::String::Format("Общая сумма выплат: {0:F2} руб.", total);

    // Очищаем таблицы
    dataGridEmployees->Rows->Clear();
    dataGridEmployees->Columns->Clear();
    dataGridWorkTypes->Rows->Clear();
    dataGridWorkTypes->Columns->Clear();

    // Настраиваем таблицу сотрудников
    dataGridEmployees->Columns->Add("colName", "ФИО");
    dataGridEmployees->Columns->Add("colPosition", "Должность");
    dataGridEmployees->Columns->Add("colSalary", "Оклад");
    dataGridEmployees->Columns->Add("colDays", "Дни");
    dataGridEmployees->Columns->Add("colTotal", "Зарплата");

    // Заполняем сотрудников
    for each (System::Collections::Generic::KeyValuePair<System::String^, Employee^> emp in dataManager->Employees) {
        dataGridEmployees->Rows->Add(
            emp.Value->FullName,
            emp.Value->Position->Name,
            emp.Value->Position->Salary,
            emp.Value->WorkDays,
            System::String::Format("{0:F2}", emp.Value->GetSalary())
        );
    }

    // Настраиваем таблицу должностей
    dataGridWorkTypes->Columns->Add("colName", "Название");
    dataGridWorkTypes->Columns->Add("colSalary", "Оклад");

    // Заполняем должности
    for each (System::Collections::Generic::KeyValuePair<System::String^, WorkType^> wt in dataManager->WorkTypes) {
        dataGridWorkTypes->Rows->Add(wt.Value->Name, wt.Value->Salary);
    }
}

// Обработчики событий
void MainForm::MenuLoad_Click(System::Object^ sender, System::EventArgs^ e) {
    System::Windows::Forms::OpenFileDialog^ dlg = gcnew System::Windows::Forms::OpenFileDialog();
    dlg->Filter = "Текстовые файлы (*.txt)|*.txt";

    dlg->Title = "Выберите файл сотрудников";
    if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
        System::String^ empFile = dlg->FileName;

        dlg->Title = "Выберите файл должностей";
        if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            System::String^ wtFile = dlg->FileName;

            if (dataManager->LoadFromFiles(empFile, wtFile)) {
                RefreshData();
                System::Windows::Forms::MessageBox::Show("Данные загружены!", "Успех",
                    System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);
            }
            else {
                System::Windows::Forms::MessageBox::Show("Ошибка загрузки!", "Ошибка",
                    System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
            }
        }
    }
}

void MainForm::MenuSave_Click(System::Object^ sender, System::EventArgs^ e) {
    System::Windows::Forms::SaveFileDialog^ dlg = gcnew System::Windows::Forms::SaveFileDialog();
    dlg->Filter = "Текстовые файлы (*.txt)|*.txt";

    dlg->Title = "Сохранить файл сотрудников";
    if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
        System::String^ empFile = dlg->FileName;

        dlg->Title = "Сохранить файл должностей";
        if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            System::String^ wtFile = dlg->FileName;

            if (dataManager->SaveToFiles(empFile, wtFile)) {
                System::Windows::Forms::MessageBox::Show("Данные сохранены!", "Успех",
                    System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);
            }
            else {
                System::Windows::Forms::MessageBox::Show("Ошибка сохранения!", "Ошибка",
                    System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
            }
        }
    }
}

void MainForm::MenuExit_Click(System::Object^ sender, System::EventArgs^ e) {
    System::Windows::Forms::Application::Exit();
}

void MainForm::AddEmployee_Click(System::Object^ sender, System::EventArgs^ e) {
    EditForm^ form = gcnew EditForm(FormMode::AddEmployee);
    if (form->ShowDialog() == System::Windows::Forms::DialogResult::OK && form->IsOK) {
        if (dataManager->AddEmployee(form->EmployeeName, form->PositionName, form->WorkDays)) {
            RefreshData();
            System::Windows::Forms::MessageBox::Show("Сотрудник добавлен!", "Успех",
                System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);
        }
        else {
            System::Windows::Forms::MessageBox::Show("Ошибка при добавлении сотрудника!", "Ошибка",
                System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
        }
    }
}

void MainForm::AddWorkType_Click(System::Object^ sender, System::EventArgs^ e) {
    EditForm^ form = gcnew EditForm(FormMode::AddWorkType);
    if (form->ShowDialog() == System::Windows::Forms::DialogResult::OK && form->IsOK) {
        if (dataManager->AddWorkType(form->EmployeeName, form->Salary)) {
            RefreshData();
            System::Windows::Forms::MessageBox::Show("Должность добавлена!", "Успех",
                System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);
        }
        else {
            System::Windows::Forms::MessageBox::Show("Ошибка при добавлении должности!", "Ошибка",
                System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
        }
    }
}

void MainForm::DeleteEmployee_Click(System::Object^ sender, System::EventArgs^ e) {
    if (dataGridEmployees->SelectedRows->Count > 0) {
        System::String^ name = dataGridEmployees->SelectedRows[0]->Cells[0]->Value->ToString();

        if (System::Windows::Forms::MessageBox::Show("Удалить сотрудника " + name + "?",
            "Подтверждение", System::Windows::Forms::MessageBoxButtons::YesNo,
            System::Windows::Forms::MessageBoxIcon::Question) == System::Windows::Forms::DialogResult::Yes) {

            if (dataManager->DeleteEmployee(name)) {
                RefreshData();
                System::Windows::Forms::MessageBox::Show("Сотрудник удален!", "Успех",
                    System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);
            }
            else {
                System::Windows::Forms::MessageBox::Show("Ошибка при удалении сотрудника!", "Ошибка",
                    System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
            }
        }
    }
}

void MainForm::DeleteWorkType_Click(System::Object^ sender, System::EventArgs^ e) {
    if (dataGridWorkTypes->SelectedRows->Count > 0) {
        System::String^ name = dataGridWorkTypes->SelectedRows[0]->Cells[0]->Value->ToString();

        if (System::Windows::Forms::MessageBox::Show("Удалить должность " + name + "?",
            "Подтверждение", System::Windows::Forms::MessageBoxButtons::YesNo,
            System::Windows::Forms::MessageBoxIcon::Question) == System::Windows::Forms::DialogResult::Yes) {

            if (dataManager->DeleteWorkType(name)) {
                RefreshData();
                System::Windows::Forms::MessageBox::Show("Должность удалена!", "Успех",
                    System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);
            }
            else {
                System::Windows::Forms::MessageBox::Show("Нельзя удалить должность, используемую сотрудниками!", "Ошибка",
                    System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
            }
        }
    }
}

void MainForm::SortByName_Click(System::Object^ sender, System::EventArgs^ e) {
    dataGridEmployees->Rows->Clear();

    System::Collections::Generic::List<Employee^>^ sorted = dataManager->GetEmployeesSortedByName();
    for each (Employee ^ emp in sorted) {
        dataGridEmployees->Rows->Add(
            emp->FullName,
            emp->Position->Name,
            emp->Position->Salary,
            emp->WorkDays,
            System::String::Format("{0:F2}", emp->GetSalary())
        );
    }

    System::Windows::Forms::MessageBox::Show("Отсортировано по имени", "Сортировка",
        System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);
}

void MainForm::SortBySalary_Click(System::Object^ sender, System::EventArgs^ e) {
    dataGridEmployees->Rows->Clear();

    System::Collections::Generic::List<Employee^>^ sorted = dataManager->GetEmployeesSortedBySalary();
    for each (Employee ^ emp in sorted) {
        dataGridEmployees->Rows->Add(
            emp->FullName,
            emp->Position->Name,
            emp->Position->Salary,
            emp->WorkDays,
            System::String::Format("{0:F2}", emp->GetSalary())
        );
    }

    System::Windows::Forms::MessageBox::Show("Отсортировано по зарплате", "Сортировка",
        System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);
}

void MainForm::ShowTotal_Click(System::Object^ sender, System::EventArgs^ e) {
    double total = dataManager->CalculateTotalSalary();
    int count = dataManager->Employees->Count;

    System::String^ message = System::String::Format(
        "Общая сумма выплат: {0:F2} руб.\n" +
        "Количество сотрудников: {1}\n" +
        "Средняя зарплата: {2:F2} руб.",
        total, count, (count > 0 ? total / count : 0)
    );

    System::Windows::Forms::MessageBox::Show(message, "Отчет по выплатам",
        System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);
}