class TransferNode:
    def __init__(self, cd3, num_inputs):
        self._in_counter = 0
        self._cd3 = cd3
        self._mixer = self._cd3.add_node("Mixer")
        self._num_inputs = num_inputs
        self._mixer.setIntParameter("num_inputs", num_inputs)
        self._out_port = (self._mixer, "out")

    def add_storage(self, storage):
        """
        This will only add half of the storage, the demand will still need to be connected.
        :param storage:
        :return:
        """
        s = self._cd3.add_node("RWHT")
        s.setDoubleParameter("storage_volume", storage["volume"])

        self._cd3.add_connection(self.out_port[0], self.out_port[1], s, "in_sw")
        self._out_port = (s, "out_sw")

        return s, "in_np"

    def link_storage(self, s):
        self._cd3.add_connection(self.out_port[0], self.out_port[1], s[0], s[1])
        self._out_port = (s[0], "out_np")

    def add_flow_probe(self):
        flow_probe = self._cd3.add_node("FlowProbe")
        self._cd3.add_connection(self._out_port[0], self._out_port[1], flow_probe, "in")
        self._out_port = (flow_probe, "out")
        return flow_probe

    @property
    def in_port(self) -> ():
        port = (self._mixer, f"in_{self._in_counter}")
        self._in_counter += 1
        return port

    @property
    def out_port(self) -> ():
        return self._out_port

