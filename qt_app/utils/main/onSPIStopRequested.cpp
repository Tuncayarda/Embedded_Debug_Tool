#include "../../mainwindow.h"
#include "../../actionEncoder.h"

void MainWindow::onSPIStopRequested()
{
    if (!m_cspiActive) return;

    sendPacket(MSG_ID_CSPI_END, QByteArray());
    m_cspiActive = false;
}

void MainWindow::onSPITerminateRequested()
{
    if (!m_cspiActive) return;

    sendPacket(MSG_ID_CSPI_TERMINATE, QByteArray());
    m_cspiActive = false;
}
