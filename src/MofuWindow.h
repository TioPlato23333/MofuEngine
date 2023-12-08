#ifndef SRC_MOFUWINDOW_
#define SRC_MOFUWINDOW_

class MofuWindow {
public:
	MofuWindow() = default;
	virtual ~MofuWindow() = default;

	void ShowWindow();

private:
	static constexpr int SCR_WIDTH = 800;
	static constexpr int SCR_HEIGHT = 600;
	static constexpr char WINDOW_NAME[] = "MofuEngine";
};

#endif  // SRC_MOFUWINDOW_
