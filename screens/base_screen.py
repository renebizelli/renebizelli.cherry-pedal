import abc

class BaseScreen(metaclass=abc.ABCMeta):

    def __init__(self, root):
        self._root = root
        self._input_service = None

    def stop_inputs(self):
        if self._input_service is not None:
            self._input_service.stop()
            self._input_service = None

    @abc.abstractclassmethod
    def destroy(self):
        pass


Base_Screen = BaseScreen
