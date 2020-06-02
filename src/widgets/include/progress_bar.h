#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <QProgressBar>

class ProgressBar {
public:
    ProgressBar();
    ProgressBar(QProgressBar *progressBar);
    void setProgress(int persent);
    void setMaximumRange(int max_range);
    void setVisibleProgressBar(bool visible);
};

#endif // PROGRESS_BAR_H
