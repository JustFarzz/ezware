#define _HAS_STD_BYTE 0
#include "todo_window.h"

#include <iostream>

#include "../ezware/ezware.h"
#include "../icons/IconsFontAwesome6.h"

extern EzWare ezWare;

TodoWindow::TodoWindow() : input_text{}, subtask_input{}, selected_task(-1) 
{
    if (!InitializeDatabase()) 
    {
        cerr << "Failed to initialize database!" << endl;
    }
    LoadTasks();
}

TodoWindow::~TodoWindow() 
{
    if (db) 
    {
        sqlite3_close(db);
    }
}

bool TodoWindow::InitializeDatabase() 
{
    int rc = sqlite3_open("farzz.db", &db);
    if (rc) return false;

    const char* sql[] = {
        "CREATE TABLE IF NOT EXISTS farzz ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "text TEXT NOT NULL,"
        "completed INTEGER DEFAULT 0);",

        "CREATE TABLE IF NOT EXISTS subtasks ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "parent_id INTEGER,"
        "text TEXT NOT NULL,"
        "completed INTEGER DEFAULT 0,"
        "FOREIGN KEY(parent_id) REFERENCES tasks(id) ON DELETE CASCADE);"
    };

    for (const char* query : sql) 
    {
        char* error_msg = nullptr;
        rc = sqlite3_exec(db, query, nullptr, nullptr, &error_msg);
        if (rc != SQLITE_OK) 
        {
            sqlite3_free(error_msg);
            return false;
        }
    }
    return true;
}

void TodoWindow::LoadTasks() 
{
    tasks.clear();

    const char* sql_main = "SELECT id, text, completed FROM farzz ORDER BY id DESC;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql_main, -1, &stmt, nullptr) != SQLITE_OK) 
    {
        cerr << "Failed to prepare main tasks statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) 
    {
        Task task;
        task.id = sqlite3_column_int(stmt, 0);
        const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        task.text = text ? string(text) : "";
        task.completed = sqlite3_column_int(stmt, 2) != 0;
        task.show_subtasks = false;
        tasks.push_back(task);
    }
    sqlite3_finalize(stmt);

    const char* sql_sub = "SELECT id, text, completed FROM subtasks WHERE parent_id = ? ORDER BY id;";
    stmt = nullptr;

    for (auto& task : tasks) 
    {
        if (sqlite3_prepare_v2(db, sql_sub, -1, &stmt, nullptr) != SQLITE_OK) 
        {
            cerr << "Failed to prepare subtasks statement: " << sqlite3_errmsg(db) << endl;
            continue;
        }

        sqlite3_bind_int(stmt, 1, task.id);

        while (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            Subtask subtask;
            subtask.id = sqlite3_column_int(stmt, 0);
            subtask.parent_id = task.id;
            const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            subtask.text = text ? string(text) : "";
            subtask.completed = sqlite3_column_int(stmt, 2) != 0;
            task.subtasks.push_back(subtask);
        }
        sqlite3_finalize(stmt);
        stmt = nullptr;
    }
}

void TodoWindow::AddTask(const string& text) 
{
    if (text.empty()) return;

    const char* sql = "INSERT INTO farzz (text, completed) VALUES (?, 0);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_text(stmt, 1, text.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
    LoadTasks();
}

void TodoWindow::AddSubtask(int parent_id, const string& text) 
{
    if (text.empty()) return;

    const char* sql = "INSERT INTO subtasks (parent_id, text, completed) VALUES (?, ?, 0);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, parent_id);
        sqlite3_bind_text(stmt, 2, text.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
    LoadTasks();
}

void TodoWindow::DeleteTask(int id) 
{
    const char* sql = "DELETE FROM farzz WHERE id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, id);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
    LoadTasks();
}

void TodoWindow::DeleteSubtask(int id) 
{
    const char* sql = "DELETE FROM subtasks WHERE id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, id);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
    LoadTasks();
}

void TodoWindow::ToggleTask(int id, bool completed) 
{
    const char* sql = "UPDATE farzz SET completed = ? WHERE id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, completed ? 1 : 0);
        sqlite3_bind_int(stmt, 2, id);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
    LoadTasks();
}

void TodoWindow::ToggleSubtask(int id, bool completed) 
{
    const char* sql = "UPDATE subtasks SET completed = ? WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;

    int prepare_result = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (prepare_result != SQLITE_OK)
    {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, completed ? 1 : 0);
    sqlite3_bind_int(stmt, 2, id);

    int step_result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (step_result != SQLITE_DONE)
    {
        cerr << "Failed to execute update: " << sqlite3_errmsg(db) << endl;
        return;
    }

    for (auto& task : tasks) 
    {
        for (auto& subtask : task.subtasks) 
        {
            if (subtask.id == id) 
            {
                subtask.completed = completed;
                return;
            }
        }
    }
}

void TodoWindow::Show(bool* p_open) 
{
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.1f, 0.1f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.1f, 0.1f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.1f, 0.1f, 0.16f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

    ImGui::SetNextWindowSize(ImVec2(900, 550));

    if (!ImGui::Begin("Todo List", p_open,
        ImGuiWindowFlags_NoResize))
    {
        ImGui::End();
        return;
    }

    if (ImGui::InputText("##NewTask", input_text, sizeof(input_text),
        ImGuiInputTextFlags_EnterReturnsTrue)) 
    {
        AddTask(input_text);
        input_text[0] = '\0';
    }

    ImGui::SameLine();
    ImVec2 addButtonSize = ImVec2(30, 30);

    ImGui::PushFont(ezWare.GetIconFont());
    if (ImGui::Button(ICON_FA_SQUARE_PLUS "", addButtonSize)) 
    {
        AddTask(input_text);
        input_text[0] = '\0';
    }
    ImGui::PopFont();

    ImGui::Separator();

    for (auto& task : tasks) 
    {
        ImGui::PushID(task.id);

        if (ImGui::ArrowButton("##arrow", task.show_subtasks ? ImGuiDir_Down : ImGuiDir_Right)) 
        {
            task.show_subtasks = !task.show_subtasks;
        }

        ImGui::SameLine();
        bool completed = task.completed;
        if (ImGui::Checkbox("##check", &completed)) 
        {
            ToggleTask(task.id, completed);
        }

        ImGui::SameLine();
        if (task.completed) 
        {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "%s", task.text.c_str());
        }
        else 
        {
            ImGui::TextColored(ImVec4(1.0f, 0.79f, 0.0f, 1.0f), "%s", task.text.c_str());
        }

        ImGui::SameLine();
        ImVec2 delTaskSize = ImVec2(30, 30);
        ImGui::PushFont(ezWare.GetIconFont());
        if (ImGui::Button(ICON_FA_TRASH "", delTaskSize)) 
        {
            DeleteTask(task.id);
        }
        ImGui::PopFont();

        if (task.show_subtasks) 
        {
            ImGui::Indent();

            ImGui::PushID("sub_input");
            if (ImGui::InputText("##NewSubtask", subtask_input, sizeof(subtask_input),
                ImGuiInputTextFlags_EnterReturnsTrue)) 
            {
                AddSubtask(task.id, subtask_input);
                subtask_input[0] = '\0';
            }

            ImGui::SameLine();

            ImGui::PushFont(ezWare.GetIconFont());
            if (ImGui::Button(ICON_FA_SQUARE_PLUS "", addButtonSize)) 
            {
                AddSubtask(task.id, subtask_input);
                subtask_input[0] = '\0';
            }
            ImGui::PopFont();
            ImGui::PopID();

            // Subtask list
            for (const auto& subtask : task.subtasks) 
            {
                ImGui::PushID(subtask.id);

                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 40);
                bool sub_completed = subtask.completed;
                if (ImGui::Checkbox("##subcheck", &sub_completed)) 
                {
                    ToggleSubtask(subtask.id, sub_completed);
                }

                ImGui::SameLine(100);
                if (subtask.completed) 
                {
                    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "%s", subtask.text.c_str());
                }
                else 
                {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.76f, 1.0f), "%s", subtask.text.c_str());
                }

                ImGui::SameLine();
                ImGui::PushFont(ezWare.GetIconFont());
                if (ImGui::Button(ICON_FA_TRASH "", delTaskSize))
                {
                    DeleteSubtask(subtask.id);
                }
                ImGui::PopFont();

                ImGui::PopID();
            }

            ImGui::Unindent();
        }

        ImGui::PopID();
    }

    ImGui::End();
    
    ImGui::PopStyleColor(5);
}