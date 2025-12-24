#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/statline.h>
#include <wx/sizer.h>
#include <wx/textdlg.h>
#include "Class.h"

enum {
    ID_ADD = 1,
    ID_EDIT,
    ID_DELETE,
    ID_SAVE,
    ID_LOAD,
    ID_SORT_NAME,
    ID_SORT_COST,
    ID_SORT_CALLS,
    ID_CALC_AVG,
    ID_CALC_TOTAL,
    ID_SHOW_STATS,
    ID_DB_EXPORT,
    ID_DB_IMPORT,
    ID_DB_INFO,
    ID_REFRESH_DB,
    ID_GRID,
    ID_NAME_TEXT,
    ID_CALLS_TEXT,
    ID_PRICE_TEXT,
    ID_TARIF_TYPE
};

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title);

private:
    wxGrid* grid;
    wxTextCtrl* nameText;
    wxTextCtrl* callsText;
    wxTextCtrl* priceText;
    wxChoice* tarifTypeChoice;
    ATC* atc;

    void OnAddClient(wxCommandEvent& event);
    void OnEditClient(wxCommandEvent& event);
    void OnDeleteClient(wxCommandEvent& event);
    void OnSaveToFile(wxCommandEvent& event);
    void OnLoadFromFile(wxCommandEvent& event);
    void OnSortByName(wxCommandEvent& event);
    void OnSortByCost(wxCommandEvent& event);
    void OnSortByCalls(wxCommandEvent& event);
    void OnCalculateAverage(wxCommandEvent& event);
    void OnCalculateTotal(wxCommandEvent& event);
    void OnShowStats(wxCommandEvent& event);
    void OnExportToDB(wxCommandEvent& event);
    void OnImportFromDB(wxCommandEvent& event);
    void OnShowDBInfo(wxCommandEvent& event);
    void OnRefreshDB(wxCommandEvent& event);
    void OnGridSelect(wxGridEvent& event);
    void UpdateGrid();
    void ClearForm();

    wxDECLARE_EVENT_TABLE();
};

class StatsFrame : public wxFrame {
public:
    StatsFrame(wxWindow* parent, float avgPrice, float totalIncome);
    StatsFrame(wxWindow* parent, float avgPrice, float totalIncome,
        int clientCount, const wxString& mostExpensiveClient, bool showDetails);

private:
    void SetupBasicUI(float avgPrice, float totalIncome);
    void SetupDetailedUI(float avgPrice, float totalIncome, int clientCount, const wxString& mostExpensiveClient);
};

class DBInfoFrame : public wxFrame {
public:
    DBInfoFrame(wxWindow* parent, const wxString& dbInfo);
};

class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_BUTTON(ID_ADD, MainFrame::OnAddClient)
EVT_BUTTON(ID_EDIT, MainFrame::OnEditClient)
EVT_BUTTON(ID_DELETE, MainFrame::OnDeleteClient)
EVT_BUTTON(ID_SAVE, MainFrame::OnSaveToFile)
EVT_BUTTON(ID_LOAD, MainFrame::OnLoadFromFile)
EVT_BUTTON(ID_SORT_NAME, MainFrame::OnSortByName)
EVT_BUTTON(ID_SORT_COST, MainFrame::OnSortByCost)
EVT_BUTTON(ID_SORT_CALLS, MainFrame::OnSortByCalls)
EVT_BUTTON(ID_CALC_AVG, MainFrame::OnCalculateAverage)
EVT_BUTTON(ID_CALC_TOTAL, MainFrame::OnCalculateTotal)
EVT_BUTTON(ID_SHOW_STATS, MainFrame::OnShowStats)
EVT_BUTTON(ID_DB_EXPORT, MainFrame::OnExportToDB)
EVT_BUTTON(ID_DB_IMPORT, MainFrame::OnImportFromDB)
EVT_BUTTON(ID_DB_INFO, MainFrame::OnShowDBInfo)
EVT_BUTTON(ID_REFRESH_DB, MainFrame::OnRefreshDB)
EVT_GRID_SELECT_CELL(MainFrame::OnGridSelect)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(1000, 650)) {

    atc = ATC::getInstance();
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer* inputSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Добавление/Редактирование клиента");
    wxFlexGridSizer* flexSizer = new wxFlexGridSizer(4, 2, 5, 5);
    flexSizer->AddGrowableCol(1, 1);

    flexSizer->Add(new wxStaticText(panel, wxID_ANY, "Имя клиента:"), 0, wxALIGN_CENTER_VERTICAL);
    nameText = new wxTextCtrl(panel, ID_NAME_TEXT);
    flexSizer->Add(nameText, 1, wxEXPAND);

    flexSizer->Add(new wxStaticText(panel, wxID_ANY, "Количество звонков:"), 0, wxALIGN_CENTER_VERTICAL);
    callsText = new wxTextCtrl(panel, ID_CALLS_TEXT);
    flexSizer->Add(callsText, 1, wxEXPAND);

    flexSizer->Add(new wxStaticText(panel, wxID_ANY, "Базовая цена тарифа:"), 0, wxALIGN_CENTER_VERTICAL);
    priceText = new wxTextCtrl(panel, ID_PRICE_TEXT);
    flexSizer->Add(priceText, 1, wxEXPAND);

    flexSizer->Add(new wxStaticText(panel, wxID_ANY, "Тип тарифа:"), 0, wxALIGN_CENTER_VERTICAL);
    tarifTypeChoice = new wxChoice(panel, ID_TARIF_TYPE);
    tarifTypeChoice->Append("Обычный (без скидки)");
    tarifTypeChoice->Append("Льготный (скидка 20%)");
    tarifTypeChoice->SetSelection(0);
    flexSizer->Add(tarifTypeChoice, 1, wxEXPAND);

    inputSizer->Add(flexSizer, 0, wxEXPAND | wxALL, 5);

    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->Add(new wxButton(panel, ID_ADD, "Добавить"), 0, wxALL, 5);
    buttonSizer->Add(new wxButton(panel, ID_EDIT, "Изменить"), 0, wxALL, 5);
    buttonSizer->Add(new wxButton(panel, ID_DELETE, "Удалить"), 0, wxALL, 5);
    buttonSizer->AddStretchSpacer();
    buttonSizer->Add(new wxButton(panel, ID_CALC_AVG, "Ср. цена"), 0, wxALL, 5);
    buttonSizer->Add(new wxButton(panel, ID_CALC_TOTAL, "Итого доход"), 0, wxALL, 5);
    buttonSizer->Add(new wxButton(panel, ID_SHOW_STATS, "Статистика"), 0, wxALL, 5);

    inputSizer->Add(buttonSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
    mainSizer->Add(inputSizer, 0, wxEXPAND | wxALL, 5);

    wxStaticBoxSizer* gridSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Список клиентов (из базы данных)");
    grid = new wxGrid(panel, ID_GRID);
    grid->CreateGrid(0, 6);
    grid->SetColLabelValue(0, "ID");
    grid->SetColLabelValue(1, "Имя");
    grid->SetColLabelValue(2, "Звонки");
    grid->SetColLabelValue(3, "Цена тарифа");
    grid->SetColLabelValue(4, "Тип тарифа");
    grid->SetColLabelValue(5, "Общая стоимость");

    grid->SetColSize(0, 60);
    grid->SetColSize(1, 150);
    grid->SetColSize(2, 100);
    grid->SetColSize(3, 120);
    grid->SetColSize(4, 150);
    grid->SetColSize(5, 150);

    gridSizer->Add(grid, 1, wxEXPAND | wxALL, 5);

    wxBoxSizer* dbButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    dbButtonSizer->Add(new wxButton(panel, ID_SORT_NAME, "Сорт. по имени"), 0, wxALL, 5);
    dbButtonSizer->Add(new wxButton(panel, ID_SORT_COST, "Сорт. по стоимости"), 0, wxALL, 5);
    dbButtonSizer->Add(new wxButton(panel, ID_SORT_CALLS, "Сорт. по звонкам"), 0, wxALL, 5);
    dbButtonSizer->AddStretchSpacer();
    dbButtonSizer->Add(new wxButton(panel, ID_DB_EXPORT, "Экспорт в CSV"), 0, wxALL, 5);
    dbButtonSizer->Add(new wxButton(panel, ID_DB_IMPORT, "Импорт из CSV"), 0, wxALL, 5);

    gridSizer->Add(dbButtonSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
    mainSizer->Add(gridSizer, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

    panel->SetSizer(mainSizer);
    Centre();
    UpdateGrid();

}

void MainFrame::OnAddClient(wxCommandEvent& event) {
    wxString name = nameText->GetValue();
    wxString callsStr = callsText->GetValue();
    wxString priceStr = priceText->GetValue();

    if (name.empty()) {
        wxMessageBox("Введите имя клиента!", "Ошибка", wxOK | wxICON_ERROR);
        return;
    }

    long calls;
    if (!callsStr.ToLong(&calls) || !ATC::validate_calls(calls)) {
        wxMessageBox("Некорректное количество звонков!", "Ошибка", wxOK | wxICON_ERROR);
        return;
    }

    double price;
    if (!priceStr.ToDouble(&price) || !ATC::validate_price(price)) {
        wxMessageBox("Некорректная цена!", "Ошибка", wxOK | wxICON_ERROR);
        return;
    }

    int tarifType = tarifTypeChoice->GetSelection();

    try {
        atc->register_client(name.ToStdString(), calls, tarifType, price);
        UpdateGrid();
        ClearForm();
        wxMessageBox("Клиент добавлен в базу данных!", "Успех", wxOK | wxICON_INFORMATION);
    }
    catch (const std::exception& e) {
        wxMessageBox(e.what(), "Ошибка", wxOK | wxICON_ERROR);
    }
}

void MainFrame::OnEditClient(wxCommandEvent& event) {
    int selectedRow = grid->GetGridCursorRow();
    if (selectedRow < 0 || selectedRow >= (int)atc->get_client_count()) {
        wxMessageBox("Выберите клиента!", "Ошибка", wxOK | wxICON_ERROR);
        return;
    }

    wxString name = nameText->GetValue();
    wxString callsStr = callsText->GetValue();
    wxString priceStr = priceText->GetValue();

    if (name.empty()) {
        wxMessageBox("Введите имя клиента!", "Ошибка", wxOK | wxICON_ERROR);
        return;
    }

    long calls;
    if (!callsStr.ToLong(&calls) || !ATC::validate_calls(calls)) {
        wxMessageBox("Некорректное количество звонков!", "Ошибка", wxOK | wxICON_ERROR);
        return;
    }

    double price;
    if (!priceStr.ToDouble(&price) || !ATC::validate_price(price)) {
        wxMessageBox("Некорректная цена!", "Ошибка", wxOK | wxICON_ERROR);
        return;
    }

    int tarifType = tarifTypeChoice->GetSelection();

    if (atc->update_client(selectedRow, name.ToStdString(), calls, tarifType, price)) {
        UpdateGrid();
        ClearForm();
        wxMessageBox("Данные обновлены в базе данных!", "Успех", wxOK | wxICON_INFORMATION);
    }
    else {
        wxMessageBox("Ошибка обновления в БД!", "Ошибка", wxOK | wxICON_ERROR);
    }
}

void MainFrame::OnDeleteClient(wxCommandEvent& event) {
    int selectedRow = grid->GetGridCursorRow();
    if (selectedRow < 0 || selectedRow >= (int)atc->get_client_count()) {
        wxMessageBox("Выберите клиента!", "Ошибка", wxOK | wxICON_ERROR);
        return;
    }

    if (wxMessageBox("Удалить клиента из базы данных?", "Подтверждение", wxYES_NO | wxICON_QUESTION) == wxYES) {
        if (atc->remove_client(selectedRow)) {
            UpdateGrid();
            ClearForm();
            wxMessageBox("Клиент удален из базы данных!", "Успех", wxOK | wxICON_INFORMATION);
        }
    }
}

void MainFrame::OnSaveToFile(wxCommandEvent& event) {
    wxFileDialog saveDialog(this, "Сохранить данные в файл", "", "clients.txt",
        "Текстовые файлы (*.txt)|*.txt|CSV файлы (*.csv)|*.csv|Все файлы (*.*)|*.*",
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (saveDialog.ShowModal() == wxID_OK) {
        wxString path = saveDialog.GetPath();
        wxString ext = path.AfterLast('.');

        if (ext.Lower() == "csv" || ext.Lower() == "txt") {
            if (atc->exportToFile(path.ToStdString())) {
                wxMessageBox("Данные экспортированы в файл!", "Успех", wxOK | wxICON_INFORMATION);
            }
            else {
                wxMessageBox("Ошибка экспорта в файл!", "Ошибка", wxOK | wxICON_ERROR);
            }
        }
        else {
            wxMessageBox("Используйте расширение .txt или .csv", "Ошибка", wxOK | wxICON_ERROR);
        }
    }
}

void MainFrame::OnLoadFromFile(wxCommandEvent& event) {
    wxFileDialog loadDialog(this, "Загрузить данные из файла", "", "",
        "Текстовые файлы (*.txt)|*.txt|CSV файлы (*.csv)|*.csv|Все файлы (*.*)|*.*",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (loadDialog.ShowModal() == wxID_OK) {
        if (wxMessageBox("Загрузить данные из файла в базу данных?\nСуществующие данные будут дополнены.",
            "Подтверждение", wxYES_NO | wxICON_QUESTION) == wxYES) {
            if (atc->importFromFile(loadDialog.GetPath().ToStdString())) {
                UpdateGrid();
                wxMessageBox("Данные импортированы в базу данных!", "Успех", wxOK | wxICON_INFORMATION);
            }
            else {
                wxMessageBox("Ошибка импорта из файла!", "Ошибка", wxOK | wxICON_ERROR);
            }
        }
    }
}

void MainFrame::OnSortByName(wxCommandEvent& event) {
    atc->sort_by_name();
    UpdateGrid();
    wxMessageBox("Отсортировано по имени", "Информация", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnSortByCost(wxCommandEvent& event) {
    atc->sort_by_cost();
    UpdateGrid();
    wxMessageBox("Отсортировано по стоимости", "Информация", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnSortByCalls(wxCommandEvent& event) {
    atc->sort_by_calls();
    UpdateGrid();
    wxMessageBox("Отсортировано по звонкам", "Информация", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnCalculateAverage(wxCommandEvent& event) {
    float avg = atc->get_average_tarif_price();
    wxMessageBox(wxString::Format("Средняя цена тарифа: %.2f", avg),
        "Средняя цена", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnCalculateTotal(wxCommandEvent& event) {
    float total = atc->get_total_income();
    wxMessageBox(wxString::Format("Общий доход: %.2f", total),
        "Общий доход", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnShowStats(wxCommandEvent& event) {
    float avg = atc->get_average_tarif_price();
    float total = atc->get_total_income();
    int count = atc->get_client_count();
    std::string mostExpensive = atc->get_most_expensive_client();

    if (count > 3) {
        new StatsFrame(this, avg, total, count, mostExpensive, true);
    }
    else {
        new StatsFrame(this, avg, total);
    }
}

void MainFrame::OnExportToDB(wxCommandEvent& event) {
    wxFileDialog exportDialog(this, "Экспорт базы данных в CSV", "", "atc_database_export.csv",
        "CSV файлы (*.csv)|*.csv|Все файлы (*.*)|*.*",
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (exportDialog.ShowModal() == wxID_OK) {
        if (atc->exportToFile(exportDialog.GetPath().ToStdString())) {
            wxMessageBox("База данных экспортирована в CSV файл!", "Успех", wxOK | wxICON_INFORMATION);
        }
        else {
            wxMessageBox("Ошибка экспорта базы данных!", "Ошибка", wxOK | wxICON_ERROR);
        }
    }
}

void MainFrame::OnImportFromDB(wxCommandEvent& event) {
    wxFileDialog importDialog(this, "Импорт данных в базу данных из CSV", "", "",
        "CSV файлы (*.csv)|*.csv|Все файлы (*.*)|*.*",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (importDialog.ShowModal() == wxID_OK) {
        if (wxMessageBox("Импортировать данные из CSV в базу данных?\nСуществующие данные будут дополнены.",
            "Подтверждение", wxYES_NO | wxICON_QUESTION) == wxYES) {
            if (atc->importFromFile(importDialog.GetPath().ToStdString())) {
                UpdateGrid();
                wxMessageBox("Данные импортированы в базу данных!", "Успех", wxOK | wxICON_INFORMATION);
            }
            else {
                wxMessageBox("Ошибка импорта в базу данных!", "Ошибка", wxOK | wxICON_ERROR);
            }
        }
    }
}

void MainFrame::OnShowDBInfo(wxCommandEvent& event) {
    std::string dbInfo = atc->getDatabaseInfo();
    new DBInfoFrame(this, dbInfo);
}

void MainFrame::OnRefreshDB(wxCommandEvent& event) {
    if (wxMessageBox("Обновить данные из базы данных?\nНесохраненные изменения будут потеряны.",
        "Подтверждение", wxYES_NO | wxICON_QUESTION) == wxYES) {
        ATC* atcInstance = ATC::getInstance();
        atcInstance->loadFromDatabase();
        UpdateGrid();
        wxMessageBox("Данные обновлены из базы данных!", "Успех", wxOK | wxICON_INFORMATION);
    }
}

void MainFrame::OnGridSelect(wxGridEvent& event) {
    int row = event.GetRow();
    if (row >= 0 && row < (int)atc->get_client_count()) {
        const TarifStrategy* client = atc->get_client(row);
        if (client) {
            nameText->SetValue(client->get_name());
            callsText->SetValue(wxString::Format("%d", client->get_number_of_calls()));
            priceText->SetValue(wxString::Format("%.2f", client->get_base_price()));
            tarifTypeChoice->SetSelection(client->get_tarif_type());
        }
    }
}

void MainFrame::UpdateGrid() {
    grid->ClearGrid();

    size_t count = atc->get_client_count();

    if (grid->GetNumberRows() > 0) {
        grid->DeleteRows(0, grid->GetNumberRows());
    }

    if (count == 0) {
        grid->AppendRows(1);
        grid->SetCellValue(0, 0, "Нет данных");
        for (int col = 0; col < 6; col++) {
            grid->SetReadOnly(0, col, true);
        }
        return;
    }

    grid->AppendRows(count);

    for (size_t i = 0; i < count; i++) {
        const TarifStrategy* client = atc->get_client(i);
        if (client) {
            grid->SetCellValue(i, 0, wxString::Format("%d", i + 1));
            grid->SetCellValue(i, 1, client->get_name());
            grid->SetCellValue(i, 2, wxString::Format("%d", client->get_number_of_calls()));
            grid->SetCellValue(i, 3, wxString::Format("%.2f", client->get_tarif_price()));
            grid->SetCellValue(i, 4, client->get_tarif_type() == 0 ? "Обычный" : "Льготный");
            grid->SetCellValue(i, 5, wxString::Format("%.2f", client->get_cost_of_calls()));

            for (int col = 0; col < 6; col++) {
                grid->SetReadOnly(i, col, true);
            }
        }
    }

    grid->AutoSizeColumns();
}

void MainFrame::ClearForm() {
    nameText->Clear();
    callsText->Clear();
    priceText->Clear();
    tarifTypeChoice->SetSelection(0);
}

StatsFrame::StatsFrame(wxWindow* parent, float avgPrice, float totalIncome)
    : wxFrame(parent, wxID_ANY, wxT("Базовая статистика"), wxDefaultPosition, wxSize(400, 250)) {

    SetupBasicUI(avgPrice, totalIncome);
    Show(true);
}

StatsFrame::StatsFrame(wxWindow* parent, float avgPrice, float totalIncome,
    int clientCount, const wxString& mostExpensiveClient, bool showDetails)
    : wxFrame(parent, wxID_ANY, wxT("Расширенная статистика"), wxDefaultPosition, wxSize(400, 350)) {

    SetupDetailedUI(avgPrice, totalIncome, clientCount, mostExpensiveClient);
    Show(true);
}

void StatsFrame::SetupBasicUI(float avgPrice, float totalIncome) {
    wxPanel* panel = new wxPanel(this);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer* statsSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Основная статистика");
    wxFlexGridSizer* flexSizer = new wxFlexGridSizer(2, 2, 10, 10);
    flexSizer->AddGrowableCol(0, 1);

    wxStaticText* title = new wxStaticText(panel, wxID_ANY, "БАЗОВАЯ СТАТИСТИКА");
    wxFont titleFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    title->SetFont(titleFont);
    statsSizer->Add(title, 0, wxALIGN_CENTER | wxALL, 10);

    flexSizer->Add(new wxStaticText(panel, wxID_ANY, "Средняя цена тарифа:"),
        0, wxALIGN_CENTER_VERTICAL);
    flexSizer->Add(new wxStaticText(panel, wxID_ANY, wxString::Format("%.2f", avgPrice)),
        0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);

    flexSizer->Add(new wxStaticText(panel, wxID_ANY, "Общий доход:"),
        0, wxALIGN_CENTER_VERTICAL);
    flexSizer->Add(new wxStaticText(panel, wxID_ANY, wxString::Format("%.2f", totalIncome)),
        0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);

    statsSizer->Add(flexSizer, 0, wxEXPAND | wxALL, 10);
    mainSizer->Add(statsSizer, 0, wxEXPAND | wxALL, 10);

    wxButton* closeBtn = new wxButton(panel, wxID_CLOSE, "Закрыть");
    mainSizer->Add(closeBtn, 0, wxALIGN_CENTER | wxALL, 10);

    panel->SetSizer(mainSizer);
    Centre();

    closeBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        Close();
        });
}

void StatsFrame::SetupDetailedUI(float avgPrice, float totalIncome, int clientCount, const wxString& mostExpensiveClient) {
    wxPanel* panel = new wxPanel(this);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer* statsSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Расширенная статистика");
    wxFlexGridSizer* flexSizer = new wxFlexGridSizer(2, 2, 10, 10);
    flexSizer->AddGrowableCol(0, 1);

    wxStaticText* title = new wxStaticText(panel, wxID_ANY, "РАСШИРЕННАЯ СТАТИСТИКА");
    wxFont titleFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    title->SetFont(titleFont);
    statsSizer->Add(title, 0, wxALIGN_CENTER | wxALL, 10);

    flexSizer->Add(new wxStaticText(panel, wxID_ANY, "Средняя цена тарифа:"),
        0, wxALIGN_CENTER_VERTICAL);
    wxStaticText* avgText = new wxStaticText(panel, wxID_ANY, wxString::Format("%.2f", avgPrice));
    flexSizer->Add(avgText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);

    flexSizer->Add(new wxStaticText(panel, wxID_ANY, "Общий доход:"),
        0, wxALIGN_CENTER_VERTICAL);
    wxStaticText* totalText = new wxStaticText(panel, wxID_ANY, wxString::Format("%.2f", totalIncome));
    flexSizer->Add(totalText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);

    flexSizer->Add(new wxStaticText(panel, wxID_ANY, "Количество клиентов:"),
        0, wxALIGN_CENTER_VERTICAL);
    wxStaticText* countText = new wxStaticText(panel, wxID_ANY, wxString::Format("%d", clientCount));
    flexSizer->Add(countText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);

    flexSizer->Add(new wxStaticText(panel, wxID_ANY, "Самый дорогой клиент:"),
        0, wxALIGN_CENTER_VERTICAL);
    wxStaticText* clientText = new wxStaticText(panel, wxID_ANY, mostExpensiveClient);
    flexSizer->Add(clientText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);

    statsSizer->Add(flexSizer, 0, wxEXPAND | wxALL, 10);
    mainSizer->Add(statsSizer, 0, wxEXPAND | wxALL, 10);

    wxStaticLine* line = new wxStaticLine(panel);
    mainSizer->Add(line, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);

    wxButton* closeBtn = new wxButton(panel, wxID_CLOSE, "Закрыть");
    mainSizer->Add(closeBtn, 0, wxALIGN_CENTER | wxALL, 10);

    panel->SetSizer(mainSizer);
    Centre();

    closeBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        Close();
        });
}

DBInfoFrame::DBInfoFrame(wxWindow* parent, const wxString& dbInfo)
    : wxFrame(parent, wxID_ANY, wxT("Информация о базе данных"), wxDefaultPosition, wxSize(500, 300)) {

    wxPanel* panel = new wxPanel(this);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer* infoSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Информация о БД");

    wxTextCtrl* infoText = new wxTextCtrl(panel, wxID_ANY, dbInfo,
        wxDefaultPosition, wxDefaultSize,
        wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH);

    wxFont font(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    infoText->SetFont(font);
    infoText->SetBackgroundColour(wxColour(240, 240, 240));

    infoSizer->Add(infoText, 1, wxEXPAND | wxALL, 10);
    mainSizer->Add(infoSizer, 1, wxEXPAND | wxALL, 10);

    wxButton* closeBtn = new wxButton(panel, wxID_CLOSE, "Закрыть");
    mainSizer->Add(closeBtn, 0, wxALIGN_CENTER | wxALL, 10);

    panel->SetSizer(mainSizer);
    Centre();

    closeBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        Close();
        });

    Show(true);
}

bool MyApp::OnInit() {
    MainFrame* frame = new MainFrame("Управление АТС - Тарифные планы (SQLite3 Database)");
    frame->Show(true);
    return true;
}

wxIMPLEMENT_APP(MyApp);