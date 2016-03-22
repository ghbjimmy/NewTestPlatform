TEMPLATE    = subdirs
SUBDIRS     = MainForm/MainForm \
    PlugIns/DutViewPlugin \
    PlugIns/FctViewPlugin



!static:SUBDIRS += PlugIns/DetailViewPlugin \
                   PlugIns/ScopeViewPlugin \
                   PlugIns/InteractionViewPlugin

