INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/..

SOURCES +=   $$PWD/basevideo.cpp  $$PWD/baseaudio.cpp
HEADERS  +=  $$PWD/basevideo.h    $$PWD/baseaudio.h

contains(DEFINES, PHONON) {
    SOURCES +=   $$PWD/qvideo.cpp  $$PWD/qaduio.cpp
    HEADERS  +=  $$PWD/qvideo.h    $$PWD/qaduio.h
}

contains(DEFINES, MPLAYER){
    SOURCES +=   $$PWD/mplayer.cpp
    HEADERS  +=  $$PWD/mplayer.h
}

contains(DEFINES, GSTREAMER){
    SOURCES +=   $$PWD/gstreamer.cpp
    HEADERS  +=  $$PWD/gstreamer.h
}

#IDEʼQtAVֻ֧֣ҪQtAVļзµincludeļУԼlibQtAVd1.aļlibļмɡ
contains(DEFINES, IDE){
    contains(DEFINES, WINDOWS) {
        INCLUDEPATH += $$PWD/QtAV/Windows/include/
        LIBS += $$PWD/QtAV/Windows/lib/libQtAVd1.a  \
                $$PWD/portaudio/Windows/lib/portaudio.lib
    }
}
!contains(DEFINES, IDE){
    contains(DEFINES, QtAV){
        contains(DEFINES, WINDOWS) {
            INCLUDEPATH += $$PWD/QtAV/Windows/include/
            LIBS += $$PWD/QtAV/Windows/lib/libQtAVd1.a  \
                    $$PWD/portaudio/Windows/lib/portaudio.lib
        }
        contains(DEFINES, UBUNTU) {
            INCLUDEPATH += $$PWD/QtAV/Ubuntu/include/
            LIBS += $$PWD/QtAV/Ubuntu/lib/libQtAVd1.a  \
                    $$PWD/portaudio/Ubuntu/lib/libportaudio.a
        }
        contains(DEFINES, LINUX) {
            INCLUDEPATH += $$PWD/QtAV/Linux/include/
            LIBS += $$PWD/QtAV/Linux/lib/libQtAVd1.a  \
                    $$PWD/portaudio/Linux/lib/libportaudio.a
        }
    }
}

contains(DEFINES, FFMPEG) {
    contains(DEFINES, WINDOWS) {
        LIBS += $$PWD/ffmpeg/Windows/lib/avcodec.lib \
                $$PWD/ffmpeg/Windows/lib/avfilter.lib \
                $$PWD/ffmpeg/Windows/lib/avformat.lib \
                $$PWD/ffmpeg/Windows/lib/avresample.lib \
                $$PWD/ffmpeg/Windows/lib/swscale.lib \
                $$PWD/ffmpeg/Windows/lib/avutil.lib \
                $$PWD/ffmpeg/Windows/lib/swresample.lib
    }
    contains(DEFINES, UBUNTU) {
        LIBS += $$PWD/ffmpeg/Ubuntu/lib/libavcodec.a \
                $$PWD/ffmpeg/Ubuntu/lib/libavfilter.a \
                $$PWD/ffmpeg/Ubuntu/lib/libavformat.a \
                $$PWD/ffmpeg/Ubuntu/lib/libavresample.a \
                $$PWD/ffmpeg/Ubuntu/lib/libswscale.a \
                $$PWD/ffmpeg/Ubuntu/lib/libavutil.a \
                $$PWD/ffmpeg/Ubuntu/lib/libswresample.a
    }
    contains(DEFINES, LINUX) {
        LIBS += $$PWD/ffmpeg/Linux/lib/libavcodec.a \
                $$PWD/ffmpeg/Linux/lib/libavfilter.a \
                $$PWD/ffmpeg/Linux/lib/libavformat.a \
                $$PWD/ffmpeg/Linux/lib/libavresample.a \
                $$PWD/ffmpeg/Linux/lib/libswscale.a \
                $$PWD/ffmpeg/Linux/lib/libavutil.a \
                $$PWD/ffmpeg/Linux/lib/libswresample.a
    }
}

contains(DEFINES, IDE){
    SOURCES +=  $$PWD/videoeditor.cpp \
                $$PWD/audioeditor.cpp \
                $$PWD/fft.cpp \
                $$PWD/baseaudiocodec.cpp \
                $$PWD/waveform.cpp
    HEADERS  +=  $$PWD/videoeditor.h \
                 $$PWD/audioeditor.h \
                 $$PWD/fft.h \
                 $$PWD/baseaudiocodec.h \
                 $$PWD/waveform.h
    FORMS   +=   $$PWD/videoeditor.ui \
                 $$PWD/audioeditor.ui \
                 $$PWD/waveform.ui
}

!contains(DEFINES, WINDOWS) {
    HEADERS +=  $$PWD/capturedevice.h
    SOURCES +=  $$PWD/capturedevice.cpp
}
