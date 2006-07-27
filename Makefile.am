ACLOCAL_AMFLAGS = -I m4

if GUI_OPT
QT =	addins/qtpwbsp \
	apps/Sample \
	apps/Cmds \
	apps/MdsEdit
QTPWBSP = true
endif

SUBDIRS =	framework/port \
		framework/mds_util \
		framework/bioapi_util \
		addins/dl/mds \
		framework/mds_util_api \
		addins/dl/mds_install \
		addins/bioapi_dummy_addin \
		addins/pwbsp \
		framework/h_layer/install \
		framework/h_layer \
		apps/mod_install \
		apps/NonGUI_Sample \
		apps/Test \
		include \
		imports/cdsa/v2_0/inc/ \
		${QT}

EXTRA_DIST =	00_License.htm \
		01_Readme.htm \
		09_Manifest.htm \
		10_Build.htm \
		11_Install.htm \
		12_Use.htm \
		20_Todo.htm \
		30_History.htm \
		31_Contributors.htm \
		32_Contacts.htm \
		Disclaimer \
		debian \
		m4/bnv_have_qt.m4

install-exec-hook:
	if test ! ${SKIPCONFIG}; \
	then \
		echo; \
		echo "Installing BIOAPI ..."; \
		echo; \
		echo "Module Directory Services (MDS) ..."; \
		${DESTDIR}${bindir}/mds_install -s ${DESTDIR}${libdir}; \
		echo "bioapi100..."; \
		${DESTDIR}${bindir}/mod_install -fi ${DESTDIR}${libdir}/libbioapi100.so; \
		echo "bioapi_dummy100..."; \
		${DESTDIR}${bindir}/mod_install -fi ${DESTDIR}${libdir}/libbioapi_dummy100.so; \
		echo "pwbsp..."; \
		${DESTDIR}${bindir}/mod_install -fi ${DESTDIR}${libdir}/libpwbsp.so; \
		if test ${QTPWBSP}; \
		then \
			echo "qtpwbsp..."; \
			${DESTDIR}${bindir}/mod_install -fi ${DESTDIR}${libdir}/libqtpwbsp.so; \
		fi; \
		echo "Done."; \
	fi

uninstall-local:
	rm -Rf ${DESTDIR}${MDS_PATH}

