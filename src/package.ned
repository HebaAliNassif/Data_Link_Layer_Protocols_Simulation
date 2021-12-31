package networksproject_v2;

@license(LGPL);

import ned.IdealChannel;
//
// TODO documentation
//
network Network
{
    parameters:
        int n = default(2);
        @display("bgb=443,282");
    submodules:
        nodes[n]: Node;
        coordinator: Coordinator;
    connections:
        for i=0..sizeof(nodes)-1 {
            nodes[i].in_coordinator <-- IdealChannel <-- coordinator.outs_nodes++;
            coordinator.inputs_nodes++ <-- IdealChannel <-- nodes[i].out_coordinator;
        }
        //nodes[0].in_pair <-- IdealChannel <-- nodes[1].out_pair;
        //nodes[1].in_pair <-- IdealChannel <-- nodes[0].out_pair;
        for i=0..int(((sizeof(nodes)/2)-1)) {
         nodes[i*2].in_pair <-- IdealChannel <-- nodes[i*2+1].out_pair;
         nodes[i*2+1].in_pair <-- IdealChannel <-- nodes[i*2].out_pair;
        }
}

