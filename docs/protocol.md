# RFC: Weakly consistent Group membership and discovery protocol

This document defines a protocol used by programs that wish to form a communication group, discover group membership and failure of other group members. The protocol is an implementation of previously described SWIM protocol with minor changes.

# Motivation
In distributed systems participants may not be pre-determined. That is to say that it is not desirable to have a static list of the nodes forming a cluster. This property is critical as individual nodes may be taken out for service or replace. This also enables scalability of the cluster as new nodes can be added as required if the task can benefit from new nodes.
Given that nodes can join and leave the group unpredictably, it is required for a given node to keep track of its peers.

On the other hand it may not be practical to keep track of all the members for extremely large clusters. If nodes join and leave group at a steady rate and each such event requires message exchange - number of such messages will grow with the size of a cluster limiting available bandwidth resources required for computation.

It may also be beneficial to use membership protocol on a resource constrained systems aka IoT devices.

That is why this implementation of SWIM includes a notion of peer capacity - that is maximum number of peers one given node can have / track. Note that this setting not required for to be the same for each peer. This trade-off makes this implementation a weakly consistent/ probability based as it it possible for a given node to be tracked only by one other peer to be consider a part of the group.  

Key observation here is that for a node to be part of a group it must be in peer list of at list one other node. It is not enough for a node to know other peers. This peers must know about that node as 'connections' are symmetrical.


# Messages
This version of gossip protocol is designed to be used over datagram oriented communications.
That is it is expected that messages do not carry any field for the size of the whole message. This serves to minimise the total size.
All messages start with a one byte-code of the message type. If byte-code can not be recognised - that is first byte
of the received datagram is not a valid message code - the whole datagram must be considered ill formed and dropped.
Given that protocol supports direct and indirect messages - that is a message from nodeA to nodeB can be relayed by nodeC
as in the case of an indirect ping - such messages carry target node ID and source node D.

    JoinReq[1] src[NodeInfo] token[] auth[]
    JoinNak[1] domain[8] code[8] etag[s]
    JoinRedirect[1] address[] domain[8] code[8] etag[s]
    JoinAck[1] desc: NodeInfo
    Leave[1] src[NodeInfo]
    PingRequest[1] srcId[NodeID] targetId[NodeID] ttl[1] payload[]
    PingRespose[1] srcId[NodeID] targetId[NodeID] ttl[1] payload[]
    Broadcast[1] self[NodeInfo]


## Message details
Message are divided into communication categories with regards to the state of the session. This are
 * Joining a group
 * Leaving a group
 * Ongoing gossip
 * Broadcast


### Joining a group
  Joining a group is done by node wishing to join, that is not already a member of the group it wishes to join,
by sending a `JoinReq` to one of the group members. This group member will decide if to admit a new member into a group
based on local configuration. It is assumed for the purpose of this sections that a new node already has some group details
such as an initial list of group members it can contact to request admission. Such nodes referred to as `seed`.

    JoinReq[1] src[NodeInfo] token[] auth[]

  A request sent from a non-group member to a node in order to join recipient's group.
With this request a sender introduces it self to a recipient and request to join it's group. A recipient will handle
such request in ocordence with it's configuration. Options include:
 - Ignore request. It is desirable in some situations to configure a node not to respond to join request even if
 a node could have otherwise accept the request. For example - if node must be taken our for maintenance or
 it is in the process of leaving the group - it should not accept new requests.
 - Reject request (see `JoinNak`).
 - Reject with redirection (see `JoinRedirect`).
 - Accept a join request. (see `JoinAck`).

    JoinNak[1] domain[8] code[8] etag[s]

  This message informs the request sender that it's request to join has been rejected and provides a reason for rejection.
It is up to the requester to decide what to do next.
Fields `domain` `code` and `etag` string form a common error reporting structure used by the protocol to report errors where the can be transmitted.
Note that a request can be rejected if an authentication is required and the request was sent with no authentication credentials.
In this case a code can indicate the actual reason and etag can represent a hint for a connection token.

    JoinRedirect[1] address[] domain[8] code[8] etag[s]

Similarly to `JoinNak`, but provides an alternative address requester can try to dial in order to join the group.
Note that this response imply that the request would have been accepted by the recipient if not for some run-time condition such as being at capacity.


    JoinAck[1] desc[NodeInfo]

  A connection request has been accepted and recipient acknowledged that a sender is now part of its group.
This message can be think of as a 'welcome to the group' message containing useful group details (maybe sampled)


### Leaving a group
It is a responsibility of each node to estimate group membership. Thus when node decides to leave the group - no message
is technically required. However the detection takes time and results in number of gossip rounds for information to be
propagated through a cluster. In order to simplify the process and save some bandwidth - a node may chose to send a "leave"
message, informing other group members that it is voluntarily leaving the group.

    Leave[1] src[NodeInfo]

Informs the group that node has left the group voluntarily.

### Ongoing gossip
After a node has successfully joined a group it can participate in a group gossip - a way to distribute information among peers.
All of the gossip is piggy-back on two main messages - ping request and ping response.

    PingRequest[1] srcId[NodeID] targetId[NodeID] ttl[1] payload[]

This message communicates that requester wished to check the status / health of a request target and, in case target is
present to share some information, e.g. gossip.
`src` - is the Id of the requester for target node to reply to. Note since message can be relayed to the target by other nodes
source IP address from which the message been received is not guaranteed to be the IP of the requester (src node id)
`target` - Id of the target node to check the status of.
`ttl` - in uint8 indicating if message is to be relayed. If it is 0 - the message is direct from source to the target. Otherwise
if the recipient of the message is not its target - it is allowed to forward the message to the target (if the target is in the routing table of the recipient)
subtracting 1 from ttl. Note message should not be forwarded if ttl==1 and recipient does not know target's address.
`payload` - is a gossip info that the source request wishes to share with the target. This info typically includes announcements
of the dead peers, peers that left the group, newly joined peers etc. It can also include crypto key-chain updates as
defined by extensions.

    PingRespose[1] srcId[NodeID] targetId[NodeID] ttl[1] payload[]

If the recipient of the message is the target of the ping request - it can reply with the `PingRespose` message back to
the targetId (which is not guaranteed to be the IP address where the message been received from).

`srcId` - is the id of the node that requested the ping.
`targetId` - is the Id of the ping target. That is the node replying with `PingRespose` message.
`ttl` - value from `PingRequest`. Note ping target is required to reply to the requester directly so ttl can not be
used here to route the message, however it may be useful for requester to estimate number of hops the request took.
`payload` - the data that target wishes to share with the requester.


### Broadcast
In addition to direct peer-to-peer communication, protocol has provisions for UDP broadcasting capabilities to facilitate peer discovery.
Note that broadcast may not be supported by network environment (disable on switches) and should not be relayed upon as a sole means of peer discovery.
Furthermore - broadcast may not be enabled on a peer level due to security considerations.
Thus broadcast message is only designed to announce presence of a node that can be used as a seed.

    Broadcast[1] self[NodeInfo]

Broadcasting this message announces presence of a node described in the `self` field of the message.
recipients of this message interested in Communication with the source of this message will contact the node directly.


## Communication session example:
In this example a node 'new node'@10.3.2.1 is joining a peer group via seed node 'seed peer' @ 10.3.2.17.
```
New Node -> Seed Peer: JoinReq {id: 321, gen: 0, addr: "10.3.2.1" } token[0] auth[0]
New Node <- Seed Peer: JoinNak 7115 "Token-expected" "0x1232173fdsaf"
New Node -> Seed Peer: JoinReq {id: 321, gen: 0, addr: "10.3.2.1" } "0x1232173fdsaf" "{some auth}"
New Node <- Seed Peer: JoinAck {id: 221, gen: 2, addr: "10.3.2.17" }
....

New Node -> Seed Peer: PingRequest src:{id: 321, gen: 0} target: {id: 221, gen: 3} seq: 6253 ttl:0 { deadPeers: [84], }
New Node <- Seed Peer: PingRespose src:{id: 321, gen: 0} target: {id: 221, gen: 3} seq: 6253 ttl:0 { capacity: 3, peers: 2, sample: {
members: [{id: 321, gen: 0}, {id: 763, gen: 7}]
}
}

New Node -> Seed Peer: Leave[1] src[NodeInfo]

```
