bin_PROGRAMS = QSample

BUILT_SOURCES = 	mainwindow.h \
			mainwindow.cpp \
			moc_mainwindow.cpp
			QSampeImageCollection.cpp

QSample_SOURCES =	main.cpp \
			QSampleMainWndImpl.cpp \
			QSampleMainWndImpl.h \
			mainwindow.ui.h \
			${BUILT_SOURCES}

EXTRA_DIST = 	mainwindow.ui \
		images/icon.png

QSample_LDADD =	${QT_LIBS} \
		../../framework/h_layer/libbioapi100.la

CPPFLAGS =      -I${top_srcdir}/include \
                -I${top_srcdir}/imports/cdsa/v2_0/inc \
                ${QT_CXXFLAGS}

%.h: %.ui
	${QT_UIC} $< -o $@

%.cpp: %.ui
	${QT_UIC} -impl $*.h $< -o $@

moc_%.cpp: %.h
	${QT_MOC} $< -o $@

QSampleImageCollection.cpp: images/icon.png
	${QT_UIC} -embed QSample images/icon.png -o QSampleImageCollection.cpp
