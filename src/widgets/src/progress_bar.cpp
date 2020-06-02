#include "progress_bar.h"
#include "logger.h"

CREATE_LOGGERPTR_GLOBAL(logger_ptr, "ProgressBar", "progress_bar.cpp");

static QProgressBar *progress_bar_;

ProgressBar::ProgressBar()
{
}

ProgressBar::ProgressBar(QProgressBar *progressBar)
{
    progress_bar_ = progressBar;
    setVisibleProgressBar(false);
}

void ProgressBar::setProgress(int persent)
{
    progress_bar_->setValue(persent);
}

void ProgressBar::setMaximumRange(int max_range)
{
    progress_bar_->setRange(0, max_range);
}

void ProgressBar::setVisibleProgressBar(bool visible)
{
    LOG_AUTO_TRACE(logger_ptr, "ProgressBar::setVisibleProgressBar");
    LOG_DEBUG(logger_ptr, (visible ? "Show" : "Hide"), "progress bar");

    if (visible) {
        progress_bar_->show();
    } else {
        progress_bar_->hide();
    }
}
