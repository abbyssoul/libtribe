# Introduction

It is the nature of computation that it requires some resources to be perform. Be it memory where data and intermediate results are stored or Computational units - CPU Cores or GPU Cores any physical computational system is always limited in resources.
Although it is possible to a design a system to have enough capacity for a given tasks, for many important applications resource requirements are not known a-priori. For example reactive system where the demand is unpredictable don't have such option.
In other domains it is simply not acceptable to have a single computation node susceptible to a failure - that is being a single point of failure.
Such applications can benefit from `clustering`. That is having a group of compute agents working together on a single task.

It is worth noticing that having multiple nodes does eliminate possibility of individual node failure. When viewed as a whole - chances of any single node failing as grows with the size of the cluster. What is desirable to have is the probability of the system to continue working as long as some of the nodes remain functional. This decreasing chances of the whole system failure with the size of the cluster because for a well design cluster application to fail multiple nodes
(Numbers depend on the cluster design and task) - must fail at the same time. Chances of simultaneous failure of independent nodes are less then chances of individual node to fail. That is if all cluster participants have the same non-zero probability of a failure - as the number of participant increases it is almost guaranteed that some nodes will be in a failed state during the life-cycle of a computation. But if the system is designed to handle such partial failure -
it may be able to progress. This naturally requires development of systems resilient in a face of such failures. However building it comes with its own set of challenges.

One of the challenges of designing resilient
In any practical distributed system participants can not be pre-determined. That is to say that it is not desirable to have a static list of nodes forming a cluster. This property is critical as individual nodes may be taken out for service or replaced all the while the system as a whole must continue progress. This property also enables scalability of the cluster as new nodes can be added if the task can benefit from new nodes.

For the individual nodes to form a cluster - means nodes must be able to talk to each other in order to share data (as required by a given task). In order to communicate to other nodes - a give node must know who it can talk to. That is it must know who its peers are.
Given that nodes can join and leave the group unpredictably, it is required for a given node to keep track of its peers.
In the absence of a configuration file that each node has with the list of all possible peers - one option will be to have a dedicated service to keep track of group membership. This solution, although viable - puts a lot of pressure on a single node and does not solve the problem of failure detection.
A truly distributed solution then is for each individual node to keep track of other members.


On the other hand it may not be practical to keep track of *all* the members for extremely large clusters. If nodes join and leave group at a steady rate and each such event requires message exchange - number of such messages will grow with the size of a cluster limiting available bandwidth resources required for computation.

It may also be beneficial to use membership protocol on a resource constrained systems aka IoT devices.

That is why this implementation of SWIM includes a notion of peer capacity - that is maximum number of peers one given node can have / track. Note that this setting not required for to be the same for each peer. This trade-off makes this implementation a weakly consistent/ probability based as it it possible for a given node to be tracked only by one other peer to be consider a part of the group.  

Key observation here is that for a node to be part of a group it must be in `list of peers` of at list one other node. It is not enough for a node to know other peers. This peers must know about that node as `connections` are symmetrical.


## Decisions
Designing distributed systems has a lot of challenges. This is especially clear when we recall that a computational must be able to cope with node failure.
In other words there are some choice to be made when designing cluster solutions that depend of the task at hand. An example of such question -
"is it acceptable for two nodes to compute sub-problem?" That is it maybe a requirement of the problem for a 'task' to only ever be worked on by a sigle node.
And only in case when this node failed and the failure has been detected - the task can be re-assigned to a different node.
Alternative design - is when multiple nodes compute the same task and the one who finished first submits results. Other results, if submitter, get rejected or ignored. (That is - they don't result in a change of the overall state of the system).

Yet different set must be asked about the design of the cluster it self and guaranties it provides. For example:
"Is there ever a definitive view of the cluster at any point in time?". That is is there a central register that holds a record of all the members of the cluster?
Alternative would be for each member to have partial view of the group which makes it impossible to reconstruct full membership at any given point in time.
Also implementation details question may include:
 - Is it possible for a single node to be part of multiple groups?

It may not be practical to design a universal solution for all the cases.
Refer to the [protocol RFC](protocol.md) to see how this project answers this questions.
This may render it not fit for some of the use cases.
