// Round 108 (0.2.30 #10 — the round-54 M5 progressive diagnostic-cache
// design): incremental assembler for the helper's streamed combined
// diagnostic output.
//
// The helper frames every diagnostic section exactly like
// MainWindow::extractDiagnosticSections slices it:
//
//     ========================================
//     <title>
//     ========================================
//     Diagnostic: <key>
//     Scope: ...
//     Time: ...
//     <body>
//
// The NEXT section's first divider (or the end of the stream) closes the
// current section, so a section can be committed the moment its closing
// boundary arrives — while the rest of the run is still streaming.  The
// assembled slices use the same boundaries as the authoritative end-of-run
// parser, so the progressive commits are byte-identical to the slices the
// completion path stores (the completion commit simply overwrites them).
//
// The assembler is pure and header-only so the offscreen verification
// probes can exercise it without the GUI; MainWindow.cpp drives it through
// the privileged-request output-line sink.
#pragma once

#include <QString>
#include <QStringList>

#include <functional>
#include <utility>

class ProgressiveDiagnosticAssembler
{
public:
    // Invoked once per COMPLETED section with its exact slice (frame
    // included, trailing whitespace trimmed, matching the end-of-run
    // parser's output).  `acceptsKey` decides which "Diagnostic: <key>"
    // markers participate (the combined-report marker and unknown keys are
    // skipped, and they close nothing).
    using SectionCallback = std::function<void(const QString &key, const QString &section)>;
    using SectionStartCallback = std::function<void(const QString &key)>;
    using KeyAcceptor = std::function<bool(const QString &key)>;

    ProgressiveDiagnosticAssembler(SectionCallback callback, KeyAcceptor acceptsKey)
        : m_callback(std::move(callback))
        , m_acceptsKey(std::move(acceptsKey))
    {
    }

    // Invoked when a new section's frame opens (its "Diagnostic: <key>"
    // header arrives): the section is now streaming.  The GUI uses it to
    // paint the matching Diagnostics row as running while the body streams,
    // so the run shows progress from the moment the first section starts
    // instead of only at each section's closing boundary.
    void setSectionStartCallback(SectionStartCallback callback)
    {
        m_sectionStart = std::move(callback);
    }

    // Feeds one streamed helper line.  When a new section frame starts (its
    // "Diagnostic: <key>" header arrives), the previous section is complete
    // and is reported through the callback.
    void feed(const QString &line)
    {
        if (!m_acceptsKey) {
            return;
        }
        m_lines.append(line);
        if (m_lines.size() > kMaxAssembledLines) {
            // Bound the assembly buffer: a runaway helper can never grow it
            // without limit.  Dropping the oldest lines loses the pending
            // section's frame start; that section is then skipped (the
            // completion-time parse still stores it), which degrades the
            // progressive display, never the final register.
            const int drop = m_lines.size() - kMaxAssembledLines + kDropChunk;
            m_lines = m_lines.mid(drop);
            if (m_frameStart >= 0) {
                m_frameStart -= drop;
                if (m_frameStart < 0) {
                    m_frameStart = -1;
                }
            }
        }
        if (!isMarkerLine(line)) {
            return;
        }
        const QString key = markerKey(line);
        if (!m_acceptsKey(key)) {
            return;
        }
        // The new frame's opening divider sits three lines above its header
        // (divider, title, divider, header).  A non-matching signature falls
        // back to the header line itself, mirroring the end-of-run parser's
        // fallback.
        const int count = m_lines.size();
        int newFrameStart = count - 4;
        if (newFrameStart < 0 || !isDivider(m_lines.at(count - 2))
            || !isDivider(m_lines.at(newFrameStart))
            || m_lines.at(count - 3).isEmpty()
            || isDivider(m_lines.at(count - 3))) {
            newFrameStart = count - 1;
        }
        if (!m_pendingKey.isEmpty() && m_frameStart >= 0) {
            reportSlice(m_pendingKey, m_frameStart, newFrameStart);
        }
        m_pendingKey = key;
        m_frameStart = newFrameStart;
        if (m_sectionStart) {
            m_sectionStart(key);
        }
    }

    // Closes the final pending section at the end of the stream.
    void finish()
    {
        if (!m_pendingKey.isEmpty() && m_frameStart >= 0) {
            reportSlice(m_pendingKey, m_frameStart, m_lines.size());
        }
        m_pendingKey.clear();
        m_frameStart = -1;
    }

private:
    static bool isDivider(const QString &line)
    {
        return line == QStringLiteral("========================================");
    }

    static bool isMarkerLine(const QString &line)
    {
        return line.startsWith(QStringLiteral("Diagnostic: "));
    }

    static QString markerKey(const QString &line)
    {
        return line.mid(static_cast<int>(qstrlen("Diagnostic: "))).trimmed();
    }

    void reportSlice(const QString &key, int start, int end)
    {
        if (end <= start || !m_callback) {
            return;
        }
        QStringList slice;
        slice.reserve(end - start);
        for (int index = start; index < end; ++index) {
            slice.append(m_lines.at(index));
        }
        m_callback(key, slice.join(QLatin1Char('\n')).trimmed());
    }

    // ~8 k lines of assembly headroom is orders of magnitude above the
    // largest legitimate section; the cap keeps the buffer bounded.
    static constexpr int kMaxAssembledLines = 8192;
    static constexpr int kDropChunk = 2048;

    SectionCallback m_callback;
    SectionStartCallback m_sectionStart;
    KeyAcceptor m_acceptsKey;
    QStringList m_lines;
    QString m_pendingKey;
    int m_frameStart = -1;
};
