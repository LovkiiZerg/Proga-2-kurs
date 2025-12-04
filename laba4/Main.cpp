#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/statline.h>
#include <wx/sizer.h>
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
    void OnGridSelect(wxGridEvent& event);
    void UpdateGrid();
    void ClearForm();

    wxDECLARE_EVENT_TABLE();
};

class StatsFrame : public wxFrame {
public:
    StatsFrame(wxWindow* parent, const wxString& title, float avgPrice, float totalIncome,
        int clientCount, const wxString& mostExpensiveClient);
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
EVT_GRID_SELECT_CELL(MainFrame::OnGridSelect)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(900, 600)) {

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
    tarifTypeChoice->Append("Обычный");
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

    wxStaticBoxSizer* gridSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Список клиентов");
    grid = new wxGrid(panel, ID_GRID);
    grid->CreateGrid(0, 6);
    grid->SetColLabelValue(0, "№");
    grid->SetColLabelValue(1, "Имя");
    grid->SetColLabelValue(2, "Звонки");
    grid->SetColLabelValue(3, "Цена тарифа");
    grid->SetColLabelValue(4, "Тип тарифа");
    grid->SetColLabelValue(5, "Общая стоимость");

    grid->SetColSize(0, 50);
    grid->SetColSize(1, 150);
    grid->SetColSize(2, 100);
    grid->SetColSize(3, 120);
    grid->SetColSize(4, 150);
    grid->SetColSize(5, 150);

    gridSizer->Add(grid, 1, wxEXPAND | wxALL, 5);

    wxBoxSizer* sortSizer = new wxBoxSizer(wxHORIZONTAL);
    sortSizer->Add(new wxButton(panel, ID_SORT_NAME, "Сорт. по имени"), 0, wxALL, 5);
    sortSizer->Add(new wxButton(panel, ID_SORT_COST, "Сорт. по стоимости"), 0, wxALL, 5);
    sortSizer->Add(new wxButton(panel, ID_SORT_CALLS, "Сорт. по звонкам"), 0, wxALL, 5);
    sortSizer->AddStretchSpacer();
    sortSizer->Add(new wxButton(panel, ID_SAVE, "Сохранить в файл"), 0, wxALL, 5);
    sortSizer->Add(new wxButton(panel, ID_LOAD, "Загрузить из файла"), 0, wxALL, 5);

    gridSizer->Add(sortSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
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
        wxMessageBox("Клиент добавлен!", "Успех", wxOK | wxICON_INFORMATION);
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
        wxMessageBox("Данные обновлены!", "Успех", wxOK | wxICON_INFORMATION);
    }
    else {
        wxMessageBox("Ошибка обновления!", "Ошибка", wxOK | wxICON_ERROR);
    }
}

void MainFrame::OnDeleteClient(wxCommandEvent& event) {
    int selectedRow = grid->GetGridCursorRow();
    if (selectedRow < 0 || selectedRow >= (int)atc->get_client_count()) {
        wxMessageBox("Выберите клиента!", "Ошибка", wxOK | wxICON_ERROR);
        return;
    }

    if (wxMessageBox("Удалить клиента?", "Подтверждение", wxYES_NO | wxICON_QUESTION) == wxYES) {
        if (atc->remove_client(selectedRow)) {
            UpdateGrid();
            ClearForm();
            wxMessageBox("Клиент удален!", "Успех", wxOK | wxICON_INFORMATION);
        }
    }
}

void MainFrame::OnSaveToFile(wxCommandEvent& event) {
    wxFileDialog saveDialog(this, "Сохранить данные", "", "clients.txt",
        "Текстовые файлы (*.txt)|*.txt|Все файлы (*.*)|*.*",
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (saveDialog.ShowModal() == wxID_OK) {
        if (atc->save_to_file(saveDialog.GetPath().ToStdString())) {
            wxMessageBox("Данные сохранены!", "Успех", wxOK | wxICON_INFORMATION);
        }
        else {
            wxMessageBox("Ошибка сохранения!", "Ошибка", wxOK | wxICON_ERROR);
        }
    }
}

void MainFrame::OnLoadFromFile(wxCommandEvent& event) {
    wxFileDialog loadDialog(this, "Загрузить данные", "", "",
        "Текстовые файлы (*.txt)|*.txt|Все файлы (*.*)|*.*",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (loadDialog.ShowModal() == wxID_OK) {
        if (atc->load_from_file(loadDialog.GetPath().ToStdString())) {
            UpdateGrid();
            wxMessageBox("Данные загружены!", "Успех", wxOK | wxICON_INFORMATION);
        }
        else {
            wxMessageBox("Ошибка загрузки!", "Ошибка", wxOK | wxICON_ERROR);
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

    new StatsFrame(this, "Расширенная статистика", avg, total, count, mostExpensive);
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

StatsFrame::StatsFrame(wxWindow* parent, const wxString& title, float avgPrice, float totalIncome,
    int clientCount, const wxString& mostExpensiveClient)
    : wxFrame(parent, wxID_ANY, title, wxDefaultPosition, wxSize(400, 350)) {

    wxPanel* panel = new wxPanel(this);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer* statsSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Основная статистика");
    wxFlexGridSizer* flexSizer = new wxFlexGridSizer(2, 2, 10, 10);
    flexSizer->AddGrowableCol(0, 1);

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

    wxStaticLine* line = new wxStaticLine(panel);
    mainSizer->Add(line, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

    wxStaticBoxSizer* extraSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Дополнительная статистика");
    wxFlexGridSizer* flexSizer2 = new wxFlexGridSizer(2, 2, 5, 5);
    flexSizer2->AddGrowableCol(0, 1);

    flexSizer2->Add(new wxStaticText(panel, wxID_ANY, "Количество клиентов:"),
        0, wxALIGN_CENTER_VERTICAL);
    flexSizer2->Add(new wxStaticText(panel, wxID_ANY, wxString::Format("%d", clientCount)),
        0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);

    flexSizer2->Add(new wxStaticText(panel, wxID_ANY, "Самый дорогой клиент:"),
        0, wxALIGN_CENTER_VERTICAL);
    flexSizer2->Add(new wxStaticText(panel, wxID_ANY, mostExpensiveClient),
        0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);

    extraSizer->Add(flexSizer2, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(extraSizer, 0, wxEXPAND | wxALL, 10);

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
    MainFrame* frame = new MainFrame("АТС");
    frame->Show(true);
    return true;
}

wxIMPLEMENT_APP(MyApp);