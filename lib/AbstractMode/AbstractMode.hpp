#ifndef ABSTRACT_MODE_HPP
#define ABSTRACT_MODE_HPP

#include <Arduino.h>

#include "LightService.hpp"
#include "SpeakerService.hpp"

#include "Config.h"

class AbstractMode {
    private:
        bool active = false;
        String name;
        String description;
        String author;
        String version;
        
    protected:
        unsigned long effectStartTime = 0;
        bool effectActive = false;
        unsigned long effectDuration = 3000; // Default 3 seconds, can be overridden
        
        LightService* lightService;
        SpeakerService* speakerService;
    
    public:
        AbstractMode(LightService* lightService, SpeakerService* speakerService, 
                    const String& name, const String& description, const String& author, const String& version)
            : lightService(lightService), speakerService(speakerService), 
              name(name), description(description), author(author), version(version) {}
        AbstractMode(LightService* lightService, SpeakerService* speakerService, unsigned long duration,
                    const String& name, const String& description, const String& author, const String& version)
            : lightService(lightService), speakerService(speakerService), effectDuration(duration),
              name(name), description(description), author(author), version(version) {}
        virtual ~AbstractMode() {}

        void startDonationEffect();
        void endDonationEffect();
        
        bool isDonationEffectActive() const { return effectActive; }
        unsigned long getDonationStartTime() const { return effectStartTime; }
        unsigned long getEffectDuration() const { return effectDuration; }
        
        // Metadata getters
        const String& getName() const { return name; }
        const String& getDescription() const { return description; }
        const String& getAuthor() const { return author; }
        const String& getVersion() const { return version; }
        void printModeInfo() const;

        bool isActive();
        void activate();
        void deactivate();

        virtual void donationTriggered() = 0;
        virtual void loop() = 0;
        virtual void setup() = 0;
};

#endif