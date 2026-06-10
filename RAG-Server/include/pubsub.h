//
// Created by CallMeMosaic on 10/06/2026.
//

#ifndef PUBSUB_H
#define PUBSUB_H

void pubsub_init(void);

int subscribe_client(int client_fd, const char *topic);
int unsubscribe_client(int client_fd, const char *topic);
int publish_message(int sender_fd, const char *topic, const char *message);

void remove_client_from_all_topics(int client_fd);

#endif