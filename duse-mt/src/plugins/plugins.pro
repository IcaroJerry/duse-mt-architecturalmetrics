TEMPLATE  = subdirs
CONFIG   += ordered

SUBDIRS = \
    modelinspector \

qtHaveModule(script): SUBDIRS += javascriptconsole \
                                 jsarchitecturalmetrics

qtHaveModule(quick): SUBDIRS += welcomedashboard \
                                designoptimizer \
                                concretesyntaxview \
                                umlconcretesyntax

qtHaveModule(script):qtHaveModule(quick): SUBDIRS += designspaceexplorer

SUBDIRS += architecturerecoverycore \
    gccxmlarchitecturerecoverybackend \
    acdcarchitecturerecoveryalgorithm \
    umlmodelingnotation \
    architecturerecoverypanel
