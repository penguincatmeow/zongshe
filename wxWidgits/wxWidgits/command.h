#pragma once
#include "circuit.h"
#include<vector>
#include<memory>

//deepseek说下面这个是命令基类，虽然我现在不是很懂
class Command {
public:
    virtual~Command(){}
    virtual void execute(Circuit* c) = 0;//在电路c上执行某个操作
    virtual void undo(Circuit* c) = 0;
};
class AddGateCmd :public Command {
    AddGateCmd(const wxString& type, wxPoint pos)
        : m_type(type), m_pos(pos) {
    }

    void execute(Circuit* c) override {
        m_gateId = c->addGate(m_type, m_pos);
    }
    void undo(Circuit* c) override {
        if (m_gateId != INVALID_ID) c->removeGate(m_gateId);
    }
private:
    wxString m_type;
    wxPoint  m_pos;
    int      m_gateId = INVALID_ID;

};
//移动门
class MoveGateCmd : public Command {
public:
    MoveGateCmd(int gateId, wxPoint oldPos, wxPoint newPos)
        : m_id(gateId), m_old(oldPos), m_new(newPos) {
    }

    void execute(Circuit* c) override { c->moveGate(m_id, m_new); }
    void undo(Circuit* c)    override { c->moveGate(m_id, m_old); }
private:
    int     m_id;
    wxPoint m_old, m_new;
};
//加导线
class AddWireCmd : public Command {
public:
    AddWireCmd(int fromPin, int toPin)
        : m_from(fromPin), m_to(toPin) {
    }

    void execute(Circuit* c) override {
        m_wireId = c->addWire(m_from, m_to);
    }
    void undo(Circuit* c) override {
        if (m_wireId != INVALID_ID) c->removeWire(m_wireId);
    }
private:
    int m_from, m_to;
    int m_wireId = INVALID_ID;
};
      //记录命令历史
class CommandHistory {
public:
    void push(Circuit* c, std::unique_ptr<Command> cmd) {
        cmd->execute(c);
        m_undoStack.push_back(std::move(cmd));
        m_redoStack.clear();
    }

    void undo(Circuit* c) {
        if (m_undoStack.empty()) return;
        auto cmd = std::move(m_undoStack.back());
        m_undoStack.pop_back();
        cmd->undo(c);
        m_redoStack.push_back(std::move(cmd));
    }

    void redo(Circuit* c) {
        if (m_redoStack.empty()) return;
        auto cmd = std::move(m_redoStack.back());
        m_redoStack.pop_back();
        cmd->execute(c);
        m_undoStack.push_back(std::move(cmd));
    }

    bool canUndo() const { return !m_undoStack.empty(); }
    bool canRedo() const { return !m_redoStack.empty(); }

private:
    std::vector<std::unique_ptr<Command>> m_undoStack;
    std::vector<std::unique_ptr<Command>> m_redoStack;
};
    
