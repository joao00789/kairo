def can_build(env, platform):
    # Nova2D só faz sentido com o 2D habilitado.
    return not env["disable_2d"]


def configure(env):
    pass


def get_doc_classes():
    return ["TouchJoystick2D"]


def get_doc_path():
    return "doc_classes"
