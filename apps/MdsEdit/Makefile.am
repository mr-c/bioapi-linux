bin_PROGRAMS = MdsEdit

BUILT_SOURCES =	MdsEditMainWnd.h \
		MdsModifyStringDialog.h \
		MdsModifyUintDialog.h \
		MdsModifyMultiUintDialog.h \
		MdsFindDialog.h \
		MDSEditAboutDlg.cpp \
		MdsEditMainWnd.cpp \
		MdsFindDialog.cpp \
		MdsModifyMultiUintDialog.cpp \
		MdsModifyStringDialog.cpp \
		MdsModifyUintDialog.cpp \
		MDSEditAboutDlg.h \
		MDSEditImageCollection.cpp \
		moc_MdsEditMainWnd.cpp \
		moc_MdsModifyStringDialog.cpp \
		moc_MdsModifyUintDialog.cpp \
		moc_MdsModifyMultiUintDialog.cpp \
		moc_MdsFindDialog.cpp \
		moc_MDSEditAboutDlg.cpp

MdsEdit_SOURCES =	main.cpp \
			MdsEditDoc.cpp \
			MdsEditView.cpp \
			MdsViewItems.cpp \
			MDSEditAboutDlg.h \
			MdsEditView.h  \
			stdafx.h \
			MdsEditDoc.h \
			MdsEditMainWnd.ui.h \
			MdsViewItems.h \
			${BUILT_SOURCES}

MdsEdit_LDADD =	${QT_LIBS} \
		../Cmds/libCMDS.la \
		../../framework/bioapi_util/libbioapi_util.la
		../../framework/mds_util/libmds_util.la
		../../framework/mds_util_api/libmds_util_api.la
		../../addins/dl/mds/libbioapi_mds300.la

CPPFLAGS =      -I${top_srcdir}/include \
                -I${top_srcdir}/imports/cdsa/v2_0/inc \
                ${QT_CXXFLAGS} \
		-I${top_srcdir}/apps/Cmds

%.h: %.ui
	${QT_UIC} $< -o $@

%.cpp: %.ui
	${QT_UIC} -impl $*.h $< -o $@

moc_%.cpp: %.h
	${QT_MOC} $< -o $@

images = images/filenew \
				images/fileopen \
				images/filesave \
				images/print \
				images/undo \
				images/redo \
				images/editcut \
				images/editcopy \
				images/editpaste \
				images/searchfind \
				mdsedit.png \
				blob.png \
				multiuint.png \
				string.png \
				uint.png \
				folderclosed.png \
				folderopen.png \
				root.png \
				spacer.png

EXTRA_DIST =	${images} \
		MDSEditAboutDlg.ui \
		MdsEditMainWnd.ui \
		MdsFindDialog.ui \
		MdsModifyMultiUintDialog.ui \
		MdsModifyStringDialog.ui \
		MdsModifyUintDialog.ui

MDSEditImageCollection.cpp: ${images}
	${QT_UIC} -embed MdsEdit ${images} -o MDSEditImageCollection.cpp
