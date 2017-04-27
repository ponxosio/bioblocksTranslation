COPY %1\bioblocksTranslation.dll X:\bioblocksTranslation\dll_%2\bin\bioblocksTranslation.dll
COPY %1\bioblocksTranslation.lib X:\bioblocksTranslation\dll_%2\bin\bioblocksTranslation.lib

DEL /S X:\bioblocksTranslation\dll_%2\include\*.h
XCOPY /S /Y X:\bioblocksTranslation\bioblocksTranslation\*.h X:\bioblocksTranslation\dll_%2\include