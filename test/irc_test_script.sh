#!/bin/bash


# Variables
SESSION="irctest"
SERVER_IP="127.0.0.1"  # Update with your actual server IP
PORT="6667"             # Update with your desired port
PASSWORD="secret"        # Update with your server password
CHANNEL="#testingchannel123"


# Start a new tmux session
tmux kill-session
tmux new-session -d -s $SESSION
tmux setw synchronize-panes off
tmux set -g mouse on

# Pane 1: Run the IRC server
tmux wait-for -L serverOnline
tmux send-keys -t $SESSION "valgrind ./ircserv $PORT $PASSWORD" C-m
sleep 1
tmux wait-for -S serverOnline

# Split into 4 panes
tmux split-window -v -t $SESSION
tmux split-window -h -t $SESSION
tmux select-pane -t 0
tmux split-window -h -t $SESSION

# Pane 2: Connect as chanop using netcat
tmux wait-for serverOnline
tmux select-pane -t 1
tmux send-keys -t $SESSION "nc -C $SERVER_IP $PORT" C-m

tmux send-keys -t $SESSION "PASS $PASSWORD" C-m
tmux send-keys -t $SESSION "NICK chanop" C-m
tmux send-keys -t $SESSION "USER chanop 0 * :Channel Operator" C-m
tmux wait-for -L channelCreated
tmux send-keys -t $SESSION "JOIN $CHANNEL" C-m
sleep 1
tmux wait-for -S channelCreated

# Pane 3: Connect as user1 using netcat
tmux select-pane -t 2
tmux send-keys -t $SESSION "clear" C-m
tmux send-keys -t $SESSION "nc -C $SERVER_IP $PORT" C-m

tmux send-keys -t $SESSION "PASS $PASSWORD" C-m
tmux send-keys -t $SESSION "NICK user1" C-m
tmux send-keys -t $SESSION "USER user1 0 * :User One" C-m



# Pane 4: Connect as user2 using netcat
tmux wait-for channelCreated; 
tmux select-pane -t 3
tmux send-keys -t $SESSION "clear" C-m
tmux send-keys -t $SESSION "nc -C $SERVER_IP $PORT" C-m

tmux send-keys -t $SESSION "PASS $PASSWORD" C-m
tmux send-keys -t $SESSION "NICK user2" C-m
tmux send-keys -t $SESSION "USER user2 0 * :User Two" C-m


tmux wait-for channelCreated; 
tmux select-pane -t 3
tmux send-keys -t $SESSION "JOIN $CHANNEL" C-m
tmux select-pane -t 2
tmux send-keys -t $SESSION "JOIN $CHANNEL" C-m
# Message to channel

tmux select-pane -t 1
tmux send-keys -t $SESSION "PRIVMSG $CHANNEL :Welcome to my channel!" C-m
sleep 1
tmux select-pane -t 2
tmux send-keys -t $SESSION "PRIVMSG $CHANNEL :Thanks! Nice to be here" C-m

tmux select-pane -t 3
tmux send-keys -t $SESSION "PRIVMSG $CHANNEL :Yeah, nice to meet ya" C-m

# Private message to another user directly
tmux wait-for -L betrayal
tmux select-pane -t 2
tmux send-keys -t $SESSION "PRIVMSG user2 :Psst.. let's overthrow the mod" C-m

tmux select-pane -t 3
tmux send-keys -t $SESSION "PRIVMSG user1 :Yeah, let's kick him out" C-m
sleep 1

# Non mod trying to kick with wrong target nick
tmux select-pane -t 2
tmux send-keys -t $SESSION "KICK $CHANNEL chano :I am the captain now!" C-m
sleep 1
tmux send-keys -t $SESSION "PRIVMSG user2 :It didn't work!" C-m

# Non mod trying to kick with wrong target channel and nick
tmux select-pane -t 2
tmux send-keys -t $SESSION "KICK #wrongchan chano :I am the captain now!" C-m
sleep 1
tmux send-keys -t $SESSION "PRIVMSG user2 :It didn't work!" C-m

# Non mod trying to kick
tmux select-pane -t 3
tmux send-keys -t $SESSION "PRIVMSG user1 :Let me try, you got their name wrong" C-m
tmux send-keys -t $SESSION "KICK $CHANNEL chanop :Expecto you-die-now!" C-m
sleep 1
tmux wait-for -S betrayal
# Kick users from the channel using chanop
tmux select-pane -t 1
sleep 1
tmux wait-for -L payback
tmux wait-for betrayal
tmux send-keys -t $SESSION "PRIVMSG $CHANNEL :After all I've done for you.. begone!" C-m
tmux send-keys -t $SESSION "KICK #wrongchan user1" C-m
sleep 1
tmux send-keys -t $SESSION "KICK $CHANNEL user" C-m
sleep 1
tmux send-keys -t $SESSION "KICK $CHANNEL user1" C-m
sleep 1

tmux send-keys -t $SESSION "PRIVMSG $CHANNEL :And once more to make sure!" C-m
tmux send-keys -t $SESSION "KICK $CHANNEL user1" C-m
sleep 1

tmux send-keys -t $SESSION "PRIVMSG $CHANNEL :Now to deal with you.." C-m
tmux send-keys -t $SESSION "KICK $CHANNEL user2" C-m
sleep 1
tmux wait-for -S payback

tmux wait-for payback
tmux wait-for -L rejoin
tmux select-pane -t 3
tmux send-keys -t $SESSION "JOIN $CHANNEL" C-m
tmux select-pane -t 2
tmux send-keys -t $SESSION "JOIN $CHANNEL" C-m
tmux wait-for -S rejoin

tmux wait-for rejoin
tmux select-pane -t 1
tmux send-keys -t $SESSION "KICK $CHANNEL,$CHANNEL user1,user2 :Begone foul creatures!" C-m
sleep 1

tmux select-pane -t 0
tmux send-keys -t $SESSION C-c

# Attach to the tmux session
tmux select-pane -t 0
tmux attach -t $SESSION

