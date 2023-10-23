#include "raw80211.h"
#include <ESP8266WiFi.h> // this should be included with the esp8266 libs

const char bssid[] = "84:f3:eb:58:95:bd"; // you can put your mac address here(it is random everytime)
const uint8_t channel = 6; // by default, we are going with channel 6, although the pwnagotchi's ai will decide this

// define a json file for the pwnagotchi, i would define one here.
const char* jsonPayload = "{"
    "\"epoch\": 0, "
    "\"face\": \"(◕‿‿◕)\", "
    "\"identity\": \"b9210077f7c14c0651aa338c55e820e93f90110ef679648001b1cecdbffc0090\", "
    "\"name\": \"minigotchi\", "
    "\"policy\": {"
        "\"advertise\": false, "
        "\"ap_ttl\": 0, "
        "\"associate\": false, "
        "\"bored_num_epochs\": 0, "
        "\"channels\": [], "
        "\"deauth\": false, "
        "\"excited_num_epochs\": 0, "
        "\"hop_recon_time\": 0, "
        "\"max_inactive_scale\": 0, "
        "\"max_interactions\": 0, "
        "\"max_misses_for_recon\": 0, "
        "\"min_recon_time\": 0, "
        "\"min_rssi\": 0, "
        "\"recon_inactive_multiplier\": 0, "
        "\"recon_time\": 0, "
        "\"sad_num_epochs\": 0, "
        "\"sta_ttl\": 0"
    "}, "
    "\"pwnd_run\": 0, "
    "\"pwnd_tot\": 0, "
    "\"uptime\": 0, "
    "\"version\": \"0.1.0alpha\""
"}";


// main function
void setup() {
  // Might uncomment this later, we just need to be testing for now
  Serial.begin(115200);
  Raw80211::init(bssid, channel);
  Raw80211::start();
  Raw80211::register_cb(on_packet);
}

void loop() {
  // Convert the JSON string to a uint8_t array, then send it
  const uint8_t* payloadData = reinterpret_cast<const uint8_t*>(jsonPayload);
  Raw80211::send(payloadData, strlen(jsonPayload));
  delay(500); // Delayed between each payload by ms, in this case 500
}

void on_packet(const wifi_ieee80211_mac_hdr_t *hdr, signed int rssi, const uint8_t *buff, uint16_t buff_len) {

    const uint8_t *payloadData = buff;
    uint16_t payloadLength = buff_len;

    // Define the hardcoded information element IDs
    const uint8_t hardcodedPayloadID = 222;
    const uint8_t hardcodedCompressionID = 223;

    //create source and dest mac address for packet
    uint8_t sourceMac[6];
    memcpy(sourceMac, hdr->addr2, 6);

     uint8_t destinationMac[6];
    memcpy(destinationMac, hdr->addr1, 6);


    bool isBroadcast = true;
    for (int i = 0; i < 6; i++) {
        if (destinationMac[i] != 0xFF) {
            isBroadcast = false;
            break;
        }
    }

    if (!isBroadcast) {
      return;
    }

    Serial.println("Received a Wi-Fi packet for a BC Addr:");

    Serial.print("Source MAC: ");
    for (int i = 0; i < 6; i++) {
        Serial.print(sourceMac[i], HEX);
        if (i < 5) {
            Serial.print(":");
        }
    }
    Serial.println();

    Serial.print("Dest MAC: ");
    for (int i = 0; i < 6; i++) {
        Serial.print(destinationMac[i], HEX);
        if (i < 5) {
            Serial.print(":");
        }
    }
    Serial.println();

    Serial.print("RSSI: ");
    Serial.println(rssi);
    Serial.print("Packet Length: ");
    Serial.println(buff_len);




    bool compressed = false;
    for (uint16_t i = 0; i < payloadLength; ) {
        uint8_t elementID = payloadData[i++];
        uint8_t elementLength = payloadData[i++];

        if (elementID == hardcodedPayloadID) {
            // Process the payload data
            // You can access payloadData[i] through payloadData[i + elementLength - 1]

            // Example: Print the payload data
            Serial.print("Payload Data: ");
            for (int j = 0; j < elementLength; j++) {
                Serial.print(payloadData[i + j], HEX);
                Serial.print(" ");
            }
            Serial.println(); // Print a newline to end the line
            i += elementLength;
        } else if (elementID == hardcodedCompressionID) {
            compressed = true;
            // Handle compression if needed
            // Example: Perform decompression
        } else {
            // Handle other information elements if needed
            // Example: Skip or process other elements
            i += elementLength;
        }
    }

    if (compressed) {
        // Handle decompression if necessary
        Serial.println("Got compressed payload");
        return;
    }
    Serial.println(); // Print a newline to end the line

}