#include <wx/wx.h>
#include <wx/dcbuffer.h> 

// ========== 1. 自定义画布类，继承自 wxPanel ==========
class Canvas : public wxPanel {
public:
    // 构造函数
    Canvas(wxWindow* parent)
        : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxFULL_REPAINT_ON_RESIZE)  // 窗口大小改变时重绘
    {
        // 绑定绘制事件：窗口需要重绘时，调用 OnPaint
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        Bind(wxEVT_PAINT, &Canvas::OnPaint, this);
        Bind(wxEVT_LEFT_DOWN, &Canvas::OnLeftDown, this);  // 新增
        Bind(wxEVT_MOTION, &Canvas::OnMotion, this);
        Bind(wxEVT_LEFT_UP, &Canvas::OnLeftUp, this);      // 新增)
    }

private:
    int m_gateX = 200;
    int m_gateY = 200;
    int m_gateW = 60;
    int m_gateH = 40;
    //拖动状态
     bool m_dragging=false;
     wxPoint m_dragOffset;

    
    // 重写 OnPaint：在这里画网格
    void OnPaint(wxPaintEvent& event) {
        wxAutoBufferedPaintDC dc(this);      // 创建设备上下文（画笔）

        // 1. 白底
        dc.SetBackground(*wxWHITE_BRUSH);
        dc.Clear();
        //-----网格--------
        // 2. 灰色细线
        dc.SetPen(wxPen(wxColour(220, 220, 220), 1));

        int w = GetSize().x;
        int h = GetSize().y;

        // 竖线：每 20 像素一条
        for (int x = 0; x < w; x += 20) {
            dc.DrawLine(x, 0, x, h);
        }
        // 横线：每 20 像素一条
        for (int y = 0; y < h; y += 20) {
            dc.DrawLine(0, y, w, y);
        }
        int gx = m_gateX;
        int gy = m_gateY;
        int gw = m_gateW;
        int gh = m_gateH;
        
        //门的本体（圆角矩形）
        dc.SetPen(*wxBLACK_PEN);
        dc.SetBrush(*wxWHITE_BRUSH);
        dc.DrawRoundedRectangle(gx, gy, gw, gh, 5);
        //写AND文字
        dc.SetTextForeground(*wxBLACK);
        dc.DrawText("AND", gx + 15, gy + 12);
        // 3.3 画两条输入线（左边）
        int in1_y = gy + gh / 3;       // 第 1 条输入线 y 坐标
        int in2_y = gy + gh * 2 / 3;   // 第 2 条输入线 y 坐标
        dc.DrawLine(gx - 30, in1_y, gx, in1_y);   // 上输入
        dc.DrawLine(gx - 30, in2_y, gx, in2_y);   // 下输入

        // 3.4 画一条输出线（右边）
        int out_y = gy + gh / 2;       // 输出线 y 坐标（正中）
        dc.DrawLine(gx + gw, out_y, gx + gw + 30, out_y);

        // 3.5 在线的端点画小圆点（表示引脚）
        dc.SetBrush(*wxBLACK_BRUSH);
        dc.DrawCircle(gx - 30, in1_y, 3);
        dc.DrawCircle(gx - 30, in2_y, 3);
        dc.DrawCircle(gx + gw + 30, out_y, 3);


    }
    void OnLeftDown(wxMouseEvent& e) {
        wxPoint pt = e.GetPosition();
        wxRect gateRect(m_gateX, m_gateY, m_gateW, m_gateH);
        if (gateRect.Contains(pt)) {
            m_dragging = true;
            m_dragOffset = pt - wxPoint(m_gateX, m_gateY);
            CaptureMouse();
        }
    }
    //鼠标移动
    void OnMotion(wxMouseEvent& e) {
        if(m_dragging&e.Dragging()){
            wxPoint pt = e.GetPosition();
            m_gateX = pt.x - m_dragOffset.x;
            m_gateY = pt.y - m_dragOffset.y;
            Refresh();  // 请求重绘
        }
    }
    void OnLeftUp(wxMouseEvent&) {
        if (m_dragging) {
            m_dragging = false;
            if (HasCapture()) ReleaseMouse();
        }
    }

};

// ========== 2. 应用程序 ==========
class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        // 创建主窗口
        wxFrame* frame = new wxFrame(NULL, wxID_ANY, "网格画布",
            wxDefaultPosition, wxSize(900, 600));

        // 把 Canvas 放到 frame 里
        new Canvas(frame);

        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
