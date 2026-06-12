/*
 * pubsub.c
 *
 * Simple in-memory Publish/Subscribe system.
 *
 * Clients can:
 * - subscribe to topics
 * - unsubscribe from topics
 * - publish messages to topics
 *
 * Since multiple client threads can access the Pub/Sub state at the same time,
 * all topic operations are protected by a mutex.
 * @author CallMeMosaic
 */

#include "../include/pubsub.h"
#include "../include/config.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    char name[TOPIC_NAME_SIZE];
    int subscribers[MAX_SUBSCRIBERS_PER_TOPIC];
    int subscriber_count;
} Topic;

static Topic topics[MAX_TOPICS];
static int topic_count = 0;

static pthread_mutex_t pubsub_mutex = PTHREAD_MUTEX_INITIALIZER;

void pubsub_init(void) {
    pthread_mutex_lock(&pubsub_mutex);

    topic_count = 0;
    memset(topics, 0, sizeof(topics));

    pthread_mutex_unlock(&pubsub_mutex);
}

static int find_topic_index(const char *topic) {
    for (int i = 0; i < topic_count; i++) {
        if (strcmp(topics[i].name, topic) == 0) {
            return i;
        }
    }

    return -1;
}

static int create_topic(const char *topic) {
    if (topic_count >= MAX_TOPICS) {
        return -1;
    }

    strncpy(topics[topic_count].name, topic, TOPIC_NAME_SIZE - 1);
    topics[topic_count].name[TOPIC_NAME_SIZE - 1] = '\0';
    topics[topic_count].subscriber_count = 0;

    topic_count++;

    return topic_count - 1;
}

int subscribe_client(int client_fd, const char *topic) {
    pthread_mutex_lock(&pubsub_mutex);

    int topic_index = find_topic_index(topic);

    if (topic_index == -1) {
        topic_index = create_topic(topic);

        if (topic_index == -1) {
            pthread_mutex_unlock(&pubsub_mutex);
            return 0;
        }
    }

    Topic *selected_topic = &topics[topic_index];

    for (int i = 0; i < selected_topic->subscriber_count; i++) {
        if (selected_topic->subscribers[i] == client_fd) {
            pthread_mutex_unlock(&pubsub_mutex);
            return 1;
        }
    }

    if (selected_topic->subscriber_count >= MAX_SUBSCRIBERS_PER_TOPIC) {
        pthread_mutex_unlock(&pubsub_mutex);
        return 0;
    }

    selected_topic->subscribers[selected_topic->subscriber_count] = client_fd;
    selected_topic->subscriber_count++;

    pthread_mutex_unlock(&pubsub_mutex);

    return 1;
}

int unsubscribe_client(int client_fd, const char *topic) {
    pthread_mutex_lock(&pubsub_mutex);

    int topic_index = find_topic_index(topic);

    if (topic_index == -1) {
        pthread_mutex_unlock(&pubsub_mutex);
        return 0;
    }

    Topic *selected_topic = &topics[topic_index];

    for (int i = 0; i < selected_topic->subscriber_count; i++) {
        if (selected_topic->subscribers[i] == client_fd) {
            selected_topic->subscribers[i] =
                selected_topic->subscribers[selected_topic->subscriber_count - 1];

            selected_topic->subscriber_count--;

            pthread_mutex_unlock(&pubsub_mutex);
            return 1;
        }
    }

    pthread_mutex_unlock(&pubsub_mutex);

    return 0;
}

int publish_message(int sender_fd, const char *topic, const char *message) {
    pthread_mutex_lock(&pubsub_mutex);

    int topic_index = find_topic_index(topic);

    if (topic_index == -1) {
        pthread_mutex_unlock(&pubsub_mutex);
        return 0;
    }

    Topic selected_topic = topics[topic_index];

    pthread_mutex_unlock(&pubsub_mutex);

    char outgoing_message[BUFFER_SIZE];

    snprintf(
        outgoing_message,
        sizeof(outgoing_message),
        "[PUB %s] client %d: %s\n",
        topic,
        sender_fd,
        message
    );

    for (int i = 0; i < selected_topic.subscriber_count; i++) {
        write(selected_topic.subscribers[i], outgoing_message, strlen(outgoing_message));
    }

    return selected_topic.subscriber_count;
}

void remove_client_from_all_topics(int client_fd) {
    pthread_mutex_lock(&pubsub_mutex);

    for (int i = 0; i < topic_count; i++) {
        Topic *topic = &topics[i];

        for (int j = 0; j < topic->subscriber_count; j++) {
            if (topic->subscribers[j] == client_fd) {
                topic->subscribers[j] =
                    topic->subscribers[topic->subscriber_count - 1];

                topic->subscriber_count--;
                j--;
            }
        }
    }

    pthread_mutex_unlock(&pubsub_mutex);
}