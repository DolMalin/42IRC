#include "Server.hpp"
#include "Reply.hpp"

void Server::who (User &u, const Message &msg)
{
    if (msg.argsCount () > 0)
    {
        const std::string &channelName = msg.arg (0);

        Channel *chan = findChannelByName (channelName);
        if (chan)
        {
            if ((chan->modes.isSecret || chan->modes.isPrivate) && chan->findUser (&u) == chan->joinedUsers.end ())
                return;

            for (Channel::UserIt it = chan->joinedUsers.begin (); it != chan->joinedUsers.end (); it++)
            {
                std::string flags;
                if (it->user->isAway)
                    flags += "G";
                else
                    flags += "H";
                if (it->flags.isOperator)
                    flags += "@";
                if (it->flags.hasVoicePriviledge)
                    flags += "+";

                reply (u, Reply::whoReply (channelName, it->user->username, it->user->nickname, flags));
            }

            reply (u, Reply::endOfWho (u.nickname, channelName));
        }
    }
}
