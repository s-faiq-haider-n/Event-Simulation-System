/*
CS221 Fall 23 Lab Project
Team:
King Usama          2022609
Abdullah Shabbir    2022030
Syed Faiq H. N.     2022562
--------------------------------------575 Lines of CODE---------------------------------------------------
*/
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <fstream>
#include <algorithm>
using namespace std;

// Event Type class to store predefined event types
class EventType
{
public:
    int typeId;
    string typeName;

    EventType(int id, const string &name) : typeId(id), typeName(name) {}
};

// Event class to represent individual events
class Event
{
public:
    int eventId;
    string eventName;
    string eventDescription;
    EventType *eventType;
    string eventDateTime;
    string location;
    string address;
    string organizer;
    int numParticipants;
    int capacity;
    string status;

    Event(int id, const string &name, const string &description, EventType *type, const string &dateTime,
          const string &loc, const string &addr, const string &org, int participants, int cap)
        : eventId(id), eventName(name), eventDescription(description), eventType(type),
          eventDateTime(dateTime), location(loc), address(addr), organizer(org),
          numParticipants(participants), capacity(cap), status("Upcoming") {}
};

// Node for the linked list representing the event queue
class EventNode
{
public:
    Event *event;
    EventNode *next;

    EventNode(Event *e) : event(e), next(nullptr) {}
};

// Linked list for the event queue
class EventQueue
{
public:
    EventNode *front;
    EventNode *rear;

    EventQueue() : front(nullptr), rear(nullptr) {}

    // Enqueue an event to the end of the list
    void enqueue(Event *e)
    {
        EventNode *newNode = new EventNode(e);
        if (!rear)
        {
            front = rear = newNode;
        }
        else
        {
            rear->next = newNode;
            rear = newNode;
        }
    }

    // Dequeue an event from the front of the list
    void dequeue()
    {
        if (!front)
        {
            cout << "Queue is empty." << endl;
            return;
        }

        EventNode *temp = front;
        front = front->next;
        if (!front)
        {
            rear = nullptr;
        }

        delete temp->event;
        delete temp;
    }
};

// Node for the AVL tree representing events ordered by timestamp
class AVLNode
{
public:
    Event *event;
    AVLNode *left;
    AVLNode *right;
    int height;

    AVLNode(Event *e) : event(e), left(nullptr), right(nullptr), height(1) {}
};

// AVL Tree for efficient event scheduling
class AVLTree
{
public:
    AVLNode *root;

    AVLTree() : root(nullptr) {}

    // Get the height of a node
    int getHeight(AVLNode *node)
    {
        return (node) ? node->height : 0;
    }

    // Get the balance factor of a node
    int getBalance(AVLNode *node)
    {
        return (node) ? getHeight(node->left) - getHeight(node->right) : 0;
    }

    // Right rotation for balancing
    AVLNode *rotateRight(AVLNode *y)
    {
        AVLNode *x = y->left;
        AVLNode *T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

        return x;
    }

    // Left rotation for balancing
    AVLNode *rotateLeft(AVLNode *x)
    {
        AVLNode *y = x->right;
        AVLNode *T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

        return y;
    }

    // Insert an event into the AVL tree
    AVLNode *insert(AVLNode *node, Event *e)
    {
        if (!node)
        {
            return new AVLNode(e);
        }

        if (stoi(e->eventDateTime) < stoi(node->event->eventDateTime))
        {
            node->left = insert(node->left, e);
        }
        else if (stoi(e->eventDateTime) > stoi(node->event->eventDateTime))
        {
            node->right = insert(node->right, e);
        }

        else
        {
            // Duplicate timestamps are not allowed
            cout << "Event with the same timestamp already exists." << endl;
            return node;
        }

        node->height = 1 + max(getHeight(node->left), getHeight(node->right));

        int balance = getBalance(node);

        // Left Left Case
        if (balance > 1 && e->eventDateTime < node->left->event->eventDateTime)
        {
            return rotateRight(node);
        }

        // Right Right Case
        if (balance < -1 && e->eventDateTime > node->right->event->eventDateTime)
        {
            return rotateLeft(node);
        }

        // Left Right Case
        if (balance > 1 && e->eventDateTime > node->left->event->eventDateTime)
        {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        // Right Left Case
        if (balance < -1 && e->eventDateTime < node->right->event->eventDateTime)
        {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    // Insert an event into the AVL tree and check venue availability
    void insertEvent(Event *e)
    {
        if (isVenueAvailable(e->eventDateTime, e->location))
        {
            root = insert(root, e);
        }
        else
        {
            cout << "Venue is already occupied at the specified time." << endl;
        }
    }

    // Check if the venue is available at the given time
    bool isVenueAvailable(const string &dateTime, const string &location)
    {
        AVLNode *current = root;
        while (current)
        {
            if (dateTime < current->event->eventDateTime)
            {
                current = current->left;
            }
            else if (dateTime > current->event->eventDateTime)
            {
                current = current->right;
            }
            else
            {
                // Same timestamp, check if the location is different
                return current->event->location != location;
            }
        }
        return true; // Venue is available
    }

    // Get the event with the smallest timestamp (next event)
    Event *getMinEvent(AVLNode *node)
    {
        while (node->left)
        {
            node = node->left;
        }
        return node->event;
    }

    // Delete an event from the AVL tree
    AVLNode *deleteEvent(AVLNode *root, const string &dateTime)
    {
        if (!root)
        {
            return root;
        }

        if (dateTime < root->event->eventDateTime)
        {
            root->left = deleteEvent(root->left, dateTime);
        }
        else if (dateTime > root->event->eventDateTime)
        {
            root->right = deleteEvent(root->right, dateTime);
        }
        else
        {
            if (!root->left || !root->right)
            {
                AVLNode *temp = (root->left) ? root->left : root->right;

                if (!temp)
                {
                    temp = root;
                    root = nullptr;
                }
                else
                {
                    *root = *temp;
                }

                delete temp->event;
                delete temp;
            }
            else
            {
                AVLNode *temp = findMin(root->right);
                root->event = temp->event;
                root->right = deleteEvent(root->right, temp->event->eventDateTime);
            }
        }

        if (!root)
        {
            return root;
        }

        root->height = 1 + max(getHeight(root->left), getHeight(root->right));

        int balance = getBalance(root);

        // Left Left Case
        if (balance > 1 && getBalance(root->left) >= 0)
        {
            return rotateRight(root);
        }

        // Left Right Case
        if (balance > 1 && getBalance(root->left) < 0)
        {
            root->left = rotateLeft(root->left);
            return rotateRight(root);
        }

        // Right Right Case
        if (balance < -1 && getBalance(root->right) <= 0)
        {
            return rotateLeft(root);
        }

        // Right Left Case
        if (balance < -1 && getBalance(root->right) > 0)
        {
            root->right = rotateRight(root->right);
            return rotateLeft(root);
        }

        return root;
    }

    // Find the node with the smallest timestamp in the subtree
    AVLNode *findMin(AVLNode *node)
    {
        while (node->left)
        {
            node = node->left;
        }
        return node;
    }

    // Delete an event from the AVL tree
    void deleteEvent(const string &dateTime)
    {
        if (!root)
        {
            cout << "Tree is empty." << endl;
        }
        else
        {
            root = deleteEvent(root, dateTime);
        }
    }
};

// Simulation system class to manage the overall simulation
class SimulationSystem
{
public:
    EventQueue eventQueue;
    AVLTree eventTree;
    int eventIdCounter;
    int eventTypeIdCounter;
    vector<EventType *> eventTypes;

    SimulationSystem() : eventIdCounter(1), eventTypeIdCounter(1) {}

    // Load predefined event types from a file
    void loadEventTypes(const string &filename)
    {
        ifstream file(filename);
        if (file.is_open())
        {
            string typeName;
            while (getline(file, typeName))
            {
                eventTypes.push_back(new EventType(eventTypeIdCounter++, typeName));
            }
            file.close();
        }
        else
        {
            cout << "Unable to open file: " << filename << endl;
        }
    }

    // Display predefined event types and let the user select one
    EventType *selectEventType()
    {
        cout << "Select an event type:" << endl;
        for (EventType *type : eventTypes)
        {
            cout << type->typeId << ". " << type->typeName << endl;
        }

        int choice;
        cin >> choice;

        if (choice >= 1 && choice <= eventTypes.size())
        {
            return eventTypes[choice - 1];
        }
        else
        {
            cout << "Invalid choice. Using the first event type by default." << endl;
            return eventTypes[0];
        }
    }

    // Create a new event based on user input
    Event *createEvent()
    {
        cout << "Enter event details:" << endl;
        cout << "Event Name: ";
        string eventName;
        cin.ignore();
        getline(cin, eventName);

        cout << "Event Description: ";
        string eventDescription;
        getline(cin, eventDescription);

        EventType *eventType = selectEventType();

        cout << "Event Date (YYYY-MM-DD): ";
        string eventDate;
        cin >> eventDate;
        cin.ignore();

        cout << "Event Time (HH:MM): ";
        string eventTime;
        cin >> eventTime;
        cin.ignore();

        string eventDateTime = eventDate + " " + eventTime;

        cout << "Location: ";
        string location;
        cin.ignore();
        getline(cin, location);

        cout << "Address: ";
        string address;
        getline(cin, address);

        cout << "Organizer: ";
        string organizer;
        getline(cin, organizer);

        cout << "Number of Participants/Attendees: ";
        int numParticipants;
        cin >> numParticipants;

        cout << "Capacity: ";
        int capacity;
        cin >> capacity;

        return new Event(eventIdCounter++, eventName, eventDescription, eventType,
                         eventDateTime, location, address, organizer, numParticipants, capacity);
    }

    // Add a new event to the simulation system
    void addEvent()
    {
        Event *newEvent = createEvent();
        eventQueue.enqueue(newEvent);
        eventTree.insertEvent(newEvent);
        cout << "Event added successfully." << endl;
    }

    // Process the next event in the simulation
    void processNextEvent()
    {
        if (eventQueue.front)
        {
            Event *nextEvent = eventTree.getMinEvent(eventTree.root);
            cout << "Processing Event: " << nextEvent->eventName << " (" << nextEvent->eventDateTime << ")" << endl;

            // Additional processing logic can be added here

            eventQueue.dequeue();
            eventTree.deleteEvent(nextEvent->eventDateTime);
            cout << "Event processed successfully." << endl;
        }
        else
        {
            cout << "No upcoming events to process." << endl;
        }
    }

    // Display the current state of the simulation
    void displaySimulationState()
    {
        cout << "Simulation State:" << endl;
        cout << "Event Queue: ";
        EventNode *current = eventQueue.front;
        while (current)
        {
            cout << current->event->eventName << " (" << current->event->eventDateTime << ") ";
            current = current->next;
        }
        cout << endl;

        cout << "Event AVL Tree (In-order): ";
        inOrderTraversal(eventTree.root);
        cout << endl;
    }

    // Perform in-order traversal of the AVL tree
    void inOrderTraversal(AVLNode *node)
    {
        if (node)
        {
            inOrderTraversal(node->left);
            cout << node->event->eventName << " (" << node->event->eventDateTime << ") ";
            inOrderTraversal(node->right);
        }
    }
};

int main()
{
    SimulationSystem simulation;
    simulation.loadEventTypes("event_types.txt");

    while (true)
    {
        cout << "\nSimulation Menu:" << endl;
        cout << "1. Add Event" << endl;
        cout << "2. Process Next Event" << endl;
        cout << "3. Display Simulation State" << endl;
        cout << "4. Exit" << endl;
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        switch (choice)
        {
        case 1:
            simulation.addEvent();
            break;
        case 2:
            simulation.processNextEvent();
            break;
        case 3:
            simulation.displaySimulationState();
            break;
        case 4:
            return 0;
        default:
            cout << "Invalid choice. Try again." << endl;
        }
    }
    return 0;
}