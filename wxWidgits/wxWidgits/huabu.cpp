#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/sizer.h>
#include <vector>

// ========== 门的数据结构 ==========
struct Gate {
    int x, y;
    int w = 60, h = 40;
    wxString type;
};

// ========== 画布 ==========
class Canvas : public wxPanel {
public:
    Canvas(wxWindow* parent)
        : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxFULL_REPAINT_ON_RESIZE)
    {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        Bind(wxEVT_PAINT, &Canvas::OnPaint, this);
        Bind(wxEVT_LEFT_DOWN, &Canvas::OnLeftDown, this);
        Bind(wxEVT_MOTION, &Canvas::OnMotion, this);
        Bind(wxEVT_LEFT_UP, &Canvas::OnLeftUp, this);
    }

    // ★ 对外提供"加门"接口
    void AddGate(const wxString& type, int x = 100, int y = 100) {
        Gate g;
        g.x = x;
        g.y = y;
        g.type = type;
        m_gates.push_back(g);
        Refresh();
    }

private:
    std::vector<Gate> m_gates;      // 所有门
    int     m_dragIndex = -1;       // 正在拖的门下标，-1 = 没在拖
    wxPoint m_dragOffset;

    // -------- 画一个门 --------
    void DrawGate(wxDC& dc, const Gate& g) {
        int gx = g.x, gy = g.y, gw = g.w, gh = g.h;

        dc.SetPen(*wxBLACK_PEN);
        dc.SetBrush(*wxWHITE_BRUSH);
        dc.DrawRoundedRectangle(gx, gy, gw, gh, 5);

        dc.SetTextForeground(*wxBLACK);
        dc.DrawText(g.type, gx + 10, gy + 12);

        int inCount = (g.type == "NOT") ? 1 : 2;
        for (int i = 0; i < inCount; ++i) {
            int iy = gy + gh * (i + 1) / (inCount + 1);
            dc.SetPen(*wxBLACK_PEN);
            dc.DrawLine(gx - 30, iy, gx, iy);
            dc.SetBrush(*wxBLACK_BRUSH);
            dc.DrawCircle(gx - 30, iy, 3);
        }

        int oy = gy + gh / 2;
        dc.SetPen(*wxBLACK_PEN);
        dc.DrawLine(gx + gw, oy, gx + gw + 30, oy);
        dc.SetBrush(*wxBLACK_BRUSH);
        dc.DrawCircle(gx + gw + 30, oy, 3);
    }

    // -------- 绘制 --------
    void OnPaint(wxPaintEvent&) {
        wxAutoBufferedPaintDC dc(this);
        dc.SetBackground(*wxWHITE_BRUSH);
        dc.Clear();

        dc.SetPen(wxPen(wxColour(220, 220, 220), 1));
        int w = GetSize().x, h = GetSize().y;
        for (int x = 0; x < w; x += 20) dc.DrawLine(x, 0, x, h);
        for (int y = 0; y < h; y += 20) dc.DrawLine(0, y, w, y);

        for (const Gate& g : m_gates) DrawGate(dc, g);
    }

    // -------- 鼠标 --------
    void OnLeftDown(wxMouseEvent& e) {
        wxPoint pt = e.GetPosition();
        for (int i = (int)m_gates.size() - 1; i >= 0; --i) {
            wxRect r(m_gates[i].x, m_gates[i].y, m_gates[i].w, m_gates[i].h);
            if (r.Contains(pt)) {
                m_dragIndex = i;
                m_dragOffset = pt - wxPoint(m_gates[i].x, m_gates[i].y);
                CaptureMouse();
                return;
            }
        }
    }

    void OnMotion(wxMouseEvent& e) {
        if (m_dragIndex >= 0 && e.Dragging()) {
            wxPoint pt = e.GetPosition();
            m_gates[m_dragIndex].x = pt.x - m_dragOffset.x;
            m_gates[m_dragIndex].y = pt.y - m_dragOffset.y;
            Refresh();
        }
    }

    void OnLeftUp(wxMouseEvent&) {
        if (m_dragIndex >= 0) {
            m_dragIndex = -1;
            if (HasCapture()) ReleaseMouse();
        }
    }
};

// ========== 侧边栏 ==========
class Sidebar : public wxPanel {
public:
    Sidebar(wxWindow* parent, Canvas* canvas)
        : wxPanel(parent, wxID_ANY), m_canvas(canvas)
    {
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

        wxButton* btnAnd = new wxButton(this, wxID_ANY, "AND");
        wxButton* btnOr = new wxButton(this, wxID_ANY, "OR");
        wxButton* btnNot = new wxButton(this, wxID_ANY, "NOT");

        sizer->Add(btnAnd, 0, wxALL | wxEXPAND, 5);
        sizer->Add(btnOr, 0, wxALL | wxEXPAND, 5);
        sizer->Add(btnNot, 0, wxALL | wxEXPAND, 5);

        btnAnd->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
            m_canvas->AddGate("AND");
            });
        btnOr->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
            m_canvas->AddGate("OR");
            });
        btnNot->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
            m_canvas->AddGate("NOT");
            });

        SetSizer(sizer);
    }

private:
    Canvas* m_canvas;
};

// ========== 应用程序 ==========
class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        wxFrame* frame = new wxFrame(NULL, wxID_ANY, "Mini Logisim",
            wxDefaultPosition, wxSize(1000, 700));

        Canvas* canvas = new Canvas(frame);
        Sidebar* sidebar = new Sidebar(frame, canvas);

        wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
        sizer->Add(sidebar, 0, wxEXPAND | wxALL, 5);
        sizer->Add(canvas, 1, wxEXPAND | wxALL, 5);

        frame->SetSizer(sizer);
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);

