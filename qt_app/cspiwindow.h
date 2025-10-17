#ifndef CSPIWINDOW_H
#define CSPIWINDOW_H

#include <QWidget>

namespace Ui {
class CSPIWindow;
}

/*------------------------------------------------------------------------------
 * CSPIWindow
 *------------------------------------------------------------------------------
 * Purpose
 *   - Standalone widget that builds and manages the payload for a “Custom SPI”
 *     (CSPI) transfer workflow in the UI layer.
 *   - Collects user inputs (mode, word size, thresholds, TX data, etc.), forms
 *     a compact binary payload, and emits it to the application when requested.
 *   - Provides controls to clear/import data, stop an ongoing session, or
 *     terminate it abruptly.
 *----------------------------------------------------------------------------*/
class CSPIWindow : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Construct the CSPI widget. Parent may be nullptr for top-level use.
     */
    explicit CSPIWindow(QWidget *parent = nullptr);

    /**
     * @brief Destroy widget and its UI resources.
     */
    ~CSPIWindow();

signals:
    /**
     * @brief Emitted when the user finalizes the CSPI header + TX data.
     *        The payload format is application-defined (consumer parses it).
     */
    void payloadReady(const QByteArray& payload);

    /**
     * @brief Emitted when the user requests a graceful stop of the CSPI stream.
     *        Consumers should flush/close the session without forcing errors.
     */
    void stopRequested();

    /**
     * @brief Emitted when the user requests immediate termination of the session.
     *        Consumers may abort transfers/reset hardware as needed.
     */
    void terminateRequested();

private slots:
    /**
     * @brief Build payload from current UI state and emit @ref payloadReady.
     */
    void on_setButton_clicked();

    /**
     * @brief Clear current TX data / reset relevant UI inputs.
     */
    void on_clrButton_clicked();

    /**
     * @brief Import TX data from an external source (e.g., file dialog) and
     *        reflect it in the UI.
     */
    void on_importButton_clicked();

    /**
     * @brief Emit @ref stopRequested to perform a graceful shutdown.
     */
    void on_stopButton_clicked();

    /**
     * @brief Emit @ref terminateRequested to forcefully end the session.
     */
    void on_terminateButton_clicked();

private:
    /* Generated form instance (owned). */
    Ui::CSPIWindow *ui{nullptr};

    /**
     * @brief Update any visual indicators related to TX data (e.g., byte count,
     *        preview highlight, validity badges).
     */
    void updateTxHighlight();
};

#endif // CSPIWINDOW_H
