#include "MainForm.h"

using namespace SalaryDepartmentApp;

MainForm::MainForm() {
    dataManager = gcnew DataManager();
    currentTheme = Theme::Light;
    currentViewMode = ViewMode::Employees;

    // Инициализация компонентов в самом простом виде
    this->Text = L"Отдел расчета зарплаты";
    this->Size = System::Drawing::Size(1000, 700);
    this->StartPosition = FormStartPosition::CenterScreen;

    // Синяя панель заголовка
    System::Windows::Forms::Panel^ headerPanel = gcnew System::Windows::Forms::Panel();
    headerPanel->Dock = DockStyle::Top;
    headerPanel->Height = 70;
    headerPanel->BackColor = Color::LightBlue;

    System::Windows::Forms::Label^ title = gcnew System::Windows::Forms::Label();
    title->Text = L"Отдел расчета зарплаты \"Опушечка\"";
    title->Font = gcnew System::Drawing::Font(L"Arial", 16, FontStyle::Bold);
    title->Location = Point(10, 0);
    title->Size = System::Drawing::Size(400, 25);

    lblTotalSalary = gcnew Label();
    lblTotalSalary->Text = L"Общая сумма выплат: 0 руб.";
    lblTotalSalary->Font = gcnew System::Drawing::Font(L"Arial", 10);
    lblTotalSalary->Location = Point(10, 40);
    lblTotalSalary->Size = System::Drawing::Size(300, 20);

    headerPanel->Controls->Add(title);
    headerPanel->Controls->Add(lblTotalSalary);
    this->Controls->Add(headerPanel);

    // Меню
    System::Windows::Forms::MenuStrip^ menu = gcnew System::Windows::Forms::MenuStrip();

    // Файл
    System::Windows::Forms::ToolStripMenuItem^ fileMenu = gcnew System::Windows::Forms::ToolStripMenuItem(L"Файл");
    System::Windows::Forms::ToolStripMenuItem^ loadItem = gcnew System::Windows::Forms::ToolStripMenuItem(L"Загрузить");
    System::Windows::Forms::ToolStripMenuItem^ saveItem = gcnew System::Windows::Forms::ToolStripMenuItem(L"Сохранить");
    System::Windows::Forms::ToolStripMenuItem^ exitItem = gcnew System::Windows::Forms::ToolStripMenuItem(L"Выход");

    loadItem->Click += gcnew System::EventHandler(this, &MainForm::MenuLoad_Click);
    saveItem->Click += gcnew System::EventHandler(this, &MainForm::MenuSave_Click);
    exitItem->Click += gcnew System::EventHandler(this, &MainForm::MenuExit_Click);

    fileMenu->DropDownItems->Add(loadItem);
    fileMenu->DropDownItems->Add(saveItem);
    fileMenu->DropDownItems->Add(gcnew System::Windows::Forms::ToolStripSeparator());
    fileMenu->DropDownItems->Add(exitItem);

    // Вид
    System::Windows::Forms::ToolStripMenuItem^ viewMenu = gcnew System::Windows::Forms::ToolStripMenuItem(L"Вид");
    System::Windows::Forms::ToolStripMenuItem^ showEmpItem = gcnew System::Windows::Forms::ToolStripMenuItem(L"Показать сотрудников");
    System::Windows::Forms::ToolStripMenuItem^ showWtItem = gcnew System::Windows::Forms::ToolStripMenuItem(L"Показать должности");

    showEmpItem->Click += gcnew System::EventHandler(this, &MainForm::ShowEmployees_Click);
    showWtItem->Click += gcnew System::EventHandler(this, &MainForm::ShowWorkTypes_Click);

    viewMenu->DropDownItems->Add(showEmpItem);
    viewMenu->DropDownItems->Add(showWtItem);

    // Данные
    System::Windows::Forms::ToolStripMenuItem^ dataMenu = gcnew System::Windows::Forms::ToolStripMenuItem(L"Данные");
    System::Windows::Forms::ToolStripMenuItem^ addItem = gcnew System::Windows::Forms::ToolStripMenuItem(L"Добавить");
    System::Windows::Forms::ToolStripMenuItem^ editItem = gcnew System::Windows::Forms::ToolStripMenuItem(L"Изменить");
    System::Windows::Forms::ToolStripMenuItem^ deleteItem = gcnew System::Windows::Forms::ToolStripMenuItem(L"Удалить");

    addItem->Click += gcnew System::EventHandler(this, &MainForm::Add_Click);
    editItem->Click += gcnew System::EventHandler(this, &MainForm::Edit_Click);
    deleteItem->Click += gcnew System::EventHandler(this, &MainForm::Delete_Click);

    dataMenu->DropDownItems->Add(addItem);
    dataMenu->DropDownItems->Add(editItem);
    dataMenu->DropDownItems->Add(deleteItem);

    // Сортировка
    System::Windows::Forms::ToolStripMenuItem^ sortMenu = gcnew System::Windows::Forms::ToolStripMenuItem(L"Сортировка");
    System::Windows::Forms::ToolStripMenuItem^ sortNameItem = gcnew System::Windows::Forms::ToolStripMenuItem(L"По имени");
    System::Windows::Forms::ToolStripMenuItem^ sortSalaryItem = gcnew System::Windows::Forms::ToolStripMenuItem(L"По зарплате");

    sortNameItem->Click += gcnew System::EventHandler(this, &MainForm::SortByName_Click);
    sortSalaryItem->Click += gcnew System::EventHandler(this, &MainForm::SortBySalary_Click);

    sortMenu->DropDownItems->Add(sortNameItem);
    sortMenu->DropDownItems->Add(sortSalaryItem);

    // Отчет
    System::Windows::Forms::ToolStripMenuItem^ reportMenu = gcnew System::Windows::Forms::ToolStripMenuItem(L"Отчет");
    System::Windows::Forms::ToolStripMenuItem^ totalItem = gcnew System::Windows::Forms::ToolStripMenuItem(L"Общая сумма");

    totalItem->Click += gcnew System::EventHandler(this, &MainForm::ShowTotal_Click);
    reportMenu->DropDownItems->Add(totalItem);

    // Тема
    System::Windows::Forms::ToolStripMenuItem^ themeMenu = gcnew System::Windows::Forms::ToolStripMenuItem(L"Тема");
    System::Windows::Forms::ToolStripMenuItem^ lightThemeItem = gcnew System::Windows::Forms::ToolStripMenuItem(L"Светлая");
    System::Windows::Forms::ToolStripMenuItem^ darkThemeItem = gcnew System::Windows::Forms::ToolStripMenuItem(L"Темная");

    lightThemeItem->Click += gcnew System::EventHandler(this, &MainForm::MenuLightTheme_Click);
    darkThemeItem->Click += gcnew System::EventHandler(this, &MainForm::MenuDarkTheme_Click);

    themeMenu->DropDownItems->Add(lightThemeItem);
    themeMenu->DropDownItems->Add(darkThemeItem);

    menu->Items->Add(fileMenu);
    menu->Items->Add(viewMenu);
    menu->Items->Add(dataMenu);
    menu->Items->Add(sortMenu);
    menu->Items->Add(reportMenu);
    menu->Items->Add(themeMenu);

    this->MainMenuStrip = menu;
    this->Controls->Add(menu);

    // Таблица с фиксированным размером в середине
    dataGrid = gcnew System::Windows::Forms::DataGridView();

    // Размер таблицы (меньше размера формы)
    int tableWidth = 800;
    int tableHeight = 500;

    // Позиция для центрирования
    int tableX = (1000 - tableWidth) / 2;
    int tableY = (700 - tableHeight) / 2;

    dataGrid->Location = System::Drawing::Point(tableX, tableY);
    dataGrid->Size = System::Drawing::Size(tableWidth, tableHeight);

    // Настраиваем таблицу
    dataGrid->AllowUserToAddRows = false;
    dataGrid->ReadOnly = true;
    dataGrid->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
    dataGrid->MultiSelect = false;
    dataGrid->CellDoubleClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &MainForm::DataGrid_CellDoubleClick);

    this->Controls->Add(dataGrid);


    // Загружаем данные
    dataManager->LoadFromFiles("employees.txt", "worktypes.txt");
    RefreshData();
    ApplyTheme(currentTheme);
}

// Остальные методы остаются как в предыдущем ответе, но с System::Void
void MainForm::SwitchView(ViewMode mode) {
    currentViewMode = mode;
    RefreshData();
}

void MainForm::SetupDataGrid() {
    dataGrid->Rows->Clear();
    dataGrid->Columns->Clear();

    if (currentViewMode == ViewMode::Employees) {
        dataGrid->Columns->Add("colName", "ФИО");
        dataGrid->Columns->Add("colPosition", "Должность");
        dataGrid->Columns->Add("colSalary", "Оклад");
        dataGrid->Columns->Add("colDays", "Отработано дней");
        dataGrid->Columns->Add("colTotal", "Зарплата");

        dataGrid->Columns[0]->Width = 200;
        dataGrid->Columns[1]->Width = 150;
        dataGrid->Columns[2]->Width = 100;
        dataGrid->Columns[3]->Width = 120;
        dataGrid->Columns[4]->Width = 100;
    }
    else {
        dataGrid->Columns->Add("colName", "Название должности");
        dataGrid->Columns->Add("colSalary", "Оклад");

        dataGrid->Columns[0]->Width = 400;
        dataGrid->Columns[1]->Width = 150;
    }
}

void MainForm::RefreshData() {
    double total = dataManager->CalculateTotalSalary();
    lblTotalSalary->Text = String::Format("Общая сумма выплат: {0:F2} руб.", total);

    SetupDataGrid();

    if (currentViewMode == ViewMode::Employees) {
        for each (auto emp in dataManager->Employees) {
            dataGrid->Rows->Add(
                emp.Value->FullName,
                emp.Value->Position->Name,
                emp.Value->Position->Salary,
                emp.Value->WorkDays,
                String::Format("{0:F2}", emp.Value->GetSalary())
            );
        }
    }
    else {
        for each (auto wt in dataManager->WorkTypes) {
            dataGrid->Rows->Add(wt.Value->Name, wt.Value->Salary);
        }
    }
}

void MainForm::ApplyTheme(Theme theme) {
    currentTheme = theme;

    if (theme == Theme::Light) {
        this->BackColor = SystemColors::Control;
        this->ForeColor = SystemColors::ControlText;

        // Заголовочная панель
        Control^ headerPanel = this->Controls[0];
        headerPanel->BackColor = Color::LightBlue;
        for each (Control ^ ctrl in headerPanel->Controls) {
            ctrl->ForeColor = Color::Black;
        }

        // Меню
        if (this->MainMenuStrip != nullptr) {
            this->MainMenuStrip->BackColor = SystemColors::MenuBar;
            this->MainMenuStrip->ForeColor = SystemColors::MenuText;
        }

        // Таблица
        dataGrid->BackgroundColor = SystemColors::Window;
        dataGrid->ForeColor = SystemColors::WindowText;
        dataGrid->DefaultCellStyle->BackColor = SystemColors::Window;
        dataGrid->DefaultCellStyle->ForeColor = SystemColors::WindowText;
        dataGrid->GridColor = SystemColors::ControlDark;
    }
    else {
        this->BackColor = Color::FromArgb(45, 45, 48);
        this->ForeColor = Color::White;

        // Заголовочная панель
        Control^ headerPanel = this->Controls[0];
        headerPanel->BackColor = Color::FromArgb(0, 78, 120);
        for each (Control ^ ctrl in headerPanel->Controls) {
            ctrl->ForeColor = Color::White;
        }

        // Меню
        if (this->MainMenuStrip != nullptr) {
            this->MainMenuStrip->BackColor = Color::FromArgb(45, 45, 48);
            this->MainMenuStrip->ForeColor = Color::White;
        }

        // Таблица
        dataGrid->BackgroundColor = Color::FromArgb(45, 45, 48);
        dataGrid->ForeColor = Color::White;
        dataGrid->DefaultCellStyle->BackColor = Color::FromArgb(45, 45, 48);
        dataGrid->DefaultCellStyle->ForeColor = Color::White;
        dataGrid->GridColor = Color::Gray;
    }
}

// Обработчики переключения вида
void MainForm::ShowEmployees_Click(Object^ sender, EventArgs^ e) {
    SwitchView(ViewMode::Employees);
}

void MainForm::ShowWorkTypes_Click(Object^ sender, EventArgs^ e) {
    SwitchView(ViewMode::WorkTypes);
}

// Универсальные обработчики
void MainForm::Add_Click(Object^ sender, EventArgs^ e) {
    if (currentViewMode == ViewMode::Employees) {
        AddEmployee_Click(sender, e);
    }
    else {
        AddWorkType_Click(sender, e);
    }
}

void MainForm::Edit_Click(Object^ sender, EventArgs^ e) {
    if (currentViewMode == ViewMode::Employees) {
        EditSelectedEmployee();
    }
    else {
        EditSelectedWorkType();
    }
}

void MainForm::Delete_Click(Object^ sender, EventArgs^ e) {
    if (currentViewMode == ViewMode::Employees) {
        DeleteEmployee_Click(sender, e);
    }
    else {
        DeleteWorkType_Click(sender, e);
    }
}

void MainForm::DataGrid_CellDoubleClick(Object^ sender, DataGridViewCellEventArgs^ e) {
    if (e->RowIndex >= 0) {
        Edit_Click(sender, e);
    }
}

void MainForm::EditSelectedEmployee() {
    if (dataGrid->SelectedRows->Count > 0) {
        String^ name = dataGrid->SelectedRows[0]->Cells[0]->Value->ToString();

        EditForm^ form = gcnew EditForm(FormMode::EditEmployee, name);
        if (form->ShowDialog() == System::Windows::Forms::DialogResult::OK && form->IsOK) {
            String^ positionName = form->PositionName;

            if (!dataManager->WorkTypes->ContainsKey(positionName)) {
                MessageBox::Show("Выбранная должность не существует!", "Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
                return;
            }

            if (dataManager->UpdateEmployee(name, form->EmployeeName, positionName, form->WorkDays)) {
                RefreshData();
                MessageBox::Show("Данные сотрудника обновлены!", "Успех",
                    MessageBoxButtons::OK, MessageBoxIcon::Information);
            }
            else {
                MessageBox::Show("Ошибка при обновлении сотрудника!", "Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }
    }
    else {
        MessageBox::Show("Выберите сотрудника для редактирования!", "Внимание",
            MessageBoxButtons::OK, MessageBoxIcon::Information);
    }
}

void MainForm::EditSelectedWorkType() {
    if (dataGrid->SelectedRows->Count > 0) {
        String^ name = dataGrid->SelectedRows[0]->Cells[0]->Value->ToString();

        EditForm^ form = gcnew EditForm(FormMode::EditWorkType, name);
        if (form->ShowDialog() == System::Windows::Forms::DialogResult::OK && form->IsOK) {
            if (dataManager->UpdateWorkType(name, form->EmployeeName, form->Salary)) {
                RefreshData();
                MessageBox::Show("Данные должности обновлены!", "Успех",
                    MessageBoxButtons::OK, MessageBoxIcon::Information);
            }
            else {
                MessageBox::Show("Ошибка при обновлении должности!", "Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }
    }
    else {
        MessageBox::Show("Выберите должность для редактирования!", "Внимание",
            MessageBoxButtons::OK, MessageBoxIcon::Information);
    }
}

// Основные обработчики
void MainForm::AddEmployee_Click(Object^ sender, EventArgs^ e) {
    EditForm^ form = gcnew EditForm(FormMode::AddEmployee);
    if (form->ShowDialog() == System::Windows::Forms::DialogResult::OK && form->IsOK) {
        if (dataManager->AddEmployee(form->EmployeeName, form->PositionName, form->WorkDays)) {
            RefreshData();
            MessageBox::Show("Сотрудник добавлен!", "Успех",
                MessageBoxButtons::OK, MessageBoxIcon::Information);
        }
        else {
            MessageBox::Show("Ошибка при добавлении сотрудника!", "Ошибка",
                MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }
}

void MainForm::AddWorkType_Click(Object^ sender, EventArgs^ e) {
    EditForm^ form = gcnew EditForm(FormMode::AddWorkType);
    if (form->ShowDialog() == System::Windows::Forms::DialogResult::OK && form->IsOK) {
        if (dataManager->AddWorkType(form->EmployeeName, form->Salary)) {
            RefreshData();
            MessageBox::Show("Должность добавлена!", "Успех",
                MessageBoxButtons::OK, MessageBoxIcon::Information);
        }
        else {
            MessageBox::Show("Ошибка при добавлении должности!", "Ошибка",
                MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }
}

void MainForm::DeleteEmployee_Click(Object^ sender, EventArgs^ e) {
    if (dataGrid->SelectedRows->Count > 0) {
        String^ name = dataGrid->SelectedRows[0]->Cells[0]->Value->ToString();

        if (MessageBox::Show("Удалить сотрудника " + name + "?",
            "Подтверждение", MessageBoxButtons::YesNo,
            MessageBoxIcon::Question) == System::Windows::Forms::DialogResult::Yes) {

            if (dataManager->DeleteEmployee(name)) {
                RefreshData();
                MessageBox::Show("Сотрудник удален!", "Успех",
                    MessageBoxButtons::OK, MessageBoxIcon::Information);
            }
            else {
                MessageBox::Show("Ошибка при удалении сотрудника!", "Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }
    }
}

void MainForm::DeleteWorkType_Click(Object^ sender, EventArgs^ e) {
    if (dataGrid->SelectedRows->Count > 0) {
        String^ name = dataGrid->SelectedRows[0]->Cells[0]->Value->ToString();

        if (MessageBox::Show("Удалить должность " + name + "?",
            "Подтверждение", MessageBoxButtons::YesNo,
            MessageBoxIcon::Question) == System::Windows::Forms::DialogResult::Yes) {

            if (dataManager->DeleteWorkType(name)) {
                RefreshData();
                MessageBox::Show("Должность удалена!", "Успех",
                    MessageBoxButtons::OK, MessageBoxIcon::Information);
            }
            else {
                MessageBox::Show("Нельзя удалить должность, используемую сотрудниками!", "Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }
    }
}

void MainForm::SortByName_Click(Object^ sender, EventArgs^ e) {
    if (currentViewMode == ViewMode::Employees) {
        dataGrid->Rows->Clear();

        auto sorted = dataManager->GetEmployeesSortedByName();
        for each (auto emp in sorted) {
            dataGrid->Rows->Add(
                emp->FullName,
                emp->Position->Name,
                emp->Position->Salary,
                emp->WorkDays,
                String::Format("{0:F2}", emp->GetSalary())
            );
        }

        MessageBox::Show("Отсортировано по имени", "Сортировка",
            MessageBoxButtons::OK, MessageBoxIcon::Information);
    }
}

void MainForm::SortBySalary_Click(Object^ sender, EventArgs^ e) {
    if (currentViewMode == ViewMode::Employees) {
        dataGrid->Rows->Clear();

        auto sorted = dataManager->GetEmployeesSortedBySalary();
        for each (auto emp in sorted) {
            dataGrid->Rows->Add(
                emp->FullName,
                emp->Position->Name,
                emp->Position->Salary,
                emp->WorkDays,
                String::Format("{0:F2}", emp->GetSalary())
            );
        }

        MessageBox::Show("Отсортировано по зарплате", "Сортировка",
            MessageBoxButtons::OK, MessageBoxIcon::Information);
    }
}

void MainForm::MenuLoad_Click(Object^ sender, EventArgs^ e) {
    OpenFileDialog^ dlg = gcnew OpenFileDialog();
    dlg->Filter = "Текстовые файлы (*.txt)|*.txt";

    dlg->Title = "Выберите файл сотрудников";
    if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
        String^ empFile = dlg->FileName;

        dlg->Title = "Выберите файл должностей";
        if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            String^ wtFile = dlg->FileName;

            if (dataManager->LoadFromFiles(empFile, wtFile)) {
                RefreshData();
                MessageBox::Show("Данные загружены!", "Успех",
                    MessageBoxButtons::OK, MessageBoxIcon::Information);
            }
            else {
                MessageBox::Show("Ошибка загрузки!", "Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }
    }
}

void MainForm::MenuSave_Click(Object^ sender, EventArgs^ e) {
    SaveFileDialog^ dlg = gcnew SaveFileDialog();
    dlg->Filter = "Текстовые файлы (*.txt)|*.txt";

    dlg->Title = "Сохранить файл сотрудников";
    if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
        String^ empFile = dlg->FileName;

        dlg->Title = "Сохранить файл должностей";
        if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            String^ wtFile = dlg->FileName;

            if (dataManager->SaveToFiles(empFile, wtFile)) {
                MessageBox::Show("Данные сохранены!", "Успех",
                    MessageBoxButtons::OK, MessageBoxIcon::Information);
            }
            else {
                MessageBox::Show("Ошибка сохранения!", "Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }
    }
}

void MainForm::MenuExit_Click(Object^ sender, EventArgs^ e) {
    Application::Exit();
}

void MainForm::ShowTotal_Click(Object^ sender, EventArgs^ e) {
    double total = dataManager->CalculateTotalSalary();
    int count = dataManager->Employees->Count;

    String^ message = String::Format(
        "Общая сумма выплат: {0:F2} руб.\n" +
        "Количество сотрудников: {1}\n" +
        "Средняя зарплата: {2:F2} руб.",
        total, count, (count > 0 ? total / count : 0)
    );

    MessageBox::Show(message, "Отчет по выплатам",
        MessageBoxButtons::OK, MessageBoxIcon::Information);
}

void MainForm::MenuLightTheme_Click(Object^ sender, EventArgs^ e) {
    ApplyTheme(Theme::Light);
}

void MainForm::MenuDarkTheme_Click(Object^ sender, EventArgs^ e) {
    ApplyTheme(Theme::Dark);
}