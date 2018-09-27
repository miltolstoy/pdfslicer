// PDF Slicer
// Copyright (C) 2017-2018 Julián Unrrein

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "pagewidget.hpp"
#include <iostream>

namespace Slicer {

PageWidget::PageWidget(const Glib::RefPtr<Page>& page,
                       int targetSize,
                       bool isInteractive)
    : m_page{page}
    , m_targetSize{targetSize}
    , m_isInteractive{isInteractive}
{
    setupWidgets();

    if (m_isInteractive)
        setupSignalHandlers();
}

void PageWidget::changeSize(int targetSize)
{
    m_targetSize = targetSize;

    const Page::Size pageSize = m_page->scaledRotatedSize(m_targetSize);
    set_size_request(pageSize.width, pageSize.height);
}

void PageWidget::setupWidgets()
{
    const Page::Size pageSize = m_page->scaledRotatedSize(m_targetSize);
    set_size_request(pageSize.width, pageSize.height);
    set_valign(Gtk::ALIGN_CENTER);
    set_halign(Gtk::ALIGN_CENTER);

    if (m_isInteractive) {
        m_spinner.set_size_request(38, 38);
        m_spinner.start();
        m_contentBox.pack_start(m_spinner, true, false);

        m_check.set_halign(Gtk::ALIGN_END);
        m_check.set_valign(Gtk::ALIGN_END);
        m_check.set_margin_bottom(10);
        m_check.set_margin_right(10);

        m_previewButton.set_image_from_icon_name("document-print-preview-symbolic");
        m_previewButtonRevealer.add(m_previewButton);
        m_previewButtonRevealer.set_transition_type(Gtk::REVEALER_TRANSITION_TYPE_CROSSFADE);
        m_previewButtonRevealer.set_halign(Gtk::ALIGN_END);
        m_previewButtonRevealer.set_valign(Gtk::ALIGN_START);
        m_previewButtonRevealer.set_margin_top(10);
        m_previewButtonRevealer.set_margin_right(10);

        m_overlay.add_overlay(m_check);
        m_overlay.add_overlay(m_previewButtonRevealer);
    }

    m_overlay.set_halign(Gtk::ALIGN_CENTER);
    m_overlay.set_valign(Gtk::ALIGN_CENTER);
    m_overlay.add(m_thumbnail);
    m_overlayEventBox.add(m_overlay);

    add(m_contentBox);

    show_all();
}

void PageWidget::setupSignalHandlers()
{
    m_overlayEventBox.signal_button_release_event().connect([this](GdkEventButton* eventButton) {
        if (eventButton->button == 1) {
            if (eventButton->state & GDK_SHIFT_MASK) {
                setChecked(true);
                shiftSelected.emit(this);
            }
            else {
                setChecked(!getChecked());
                selectedChanged.emit(this);
            }

            return true;
        }

        return false;
    });

    m_overlayEventBox.signal_enter_notify_event().connect([this](GdkEventCrossing*) {
        m_previewButtonRevealer.set_reveal_child(true);

        return false;
    });

    m_overlayEventBox.signal_leave_notify_event().connect([this](GdkEventCrossing*) {
        m_previewButtonRevealer.set_reveal_child(false);

        return false;
    });

    m_previewButton.signal_enter_notify_event().connect([this](GdkEventCrossing*) {
        m_previewButtonRevealer.set_reveal_child(true);

        return false;
    });

    m_previewButton.signal_button_release_event().connect([](GdkEventButton* eventButton) {
        if (eventButton->button == 1)
            return true;

        return false;
    });

    m_previewButton.signal_clicked().connect([this]() {
        previewRequested.emit(m_page);
    });
}

void PageWidget::renderPage()
{
    m_thumbnail.set(m_page->renderPage(m_targetSize));
}

void PageWidget::showSpinner()
{
    if (!m_spinner.is_visible()) {
        m_spinner.show();
        m_spinner.start();
        m_contentBox.remove(m_overlayEventBox);
    }
}

void PageWidget::showPage()
{
    if (!isThumbnailVisible()) {
        m_spinner.stop();
        m_contentBox.pack_start(m_overlayEventBox, Gtk::PACK_SHRINK);
        m_overlayEventBox.show_all();
        m_spinner.hide();
    }
}

void PageWidget::setChecked(bool checked)
{
    if (m_isChecked != checked) {
        m_isChecked = checked;
        m_check.set_active(m_isChecked);
    }
}

bool PageWidget::isThumbnailVisible()
{
    return m_overlayEventBox.get_parent() != nullptr;
}

} // namespace Slicer
