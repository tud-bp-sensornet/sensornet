<?xml version="1.0" encoding="UTF-8"?>
<simconf>
  <project EXPORT="discard">[APPS_DIR]/mrm</project>
  <project EXPORT="discard">[APPS_DIR]/mspsim</project>
  <project EXPORT="discard">[APPS_DIR]/avrora</project>
  <project EXPORT="discard">[APPS_DIR]/serial_socket</project>
  <project EXPORT="discard">[APPS_DIR]/collect-view</project>
  <project EXPORT="discard">[APPS_DIR]/powertracker</project>
  <simulation>
    <title>01-2hop-test</title>
    <randomseed>123456</randomseed>
    <motedelay_us>1000000</motedelay_us>
    <radiomedium>
      se.sics.cooja.radiomediums.UDGM
      <transmitting_range>50.0</transmitting_range>
      <interference_range>100.0</interference_range>
      <success_ratio_tx>1.0</success_ratio_tx>
      <success_ratio_rx>1.0</success_ratio_rx>
    </radiomedium>
    <events>
      <logoutput>40000</logoutput>
    </events>
    <motetype>
      se.sics.cooja.mspmote.SkyMoteType
      <identifier>sky1</identifier>
      <description>Sky Mote Type #sky1</description>
      <source EXPORT="discard">[CONTIKI_DIR]/examples/rime/example-broadcast.c</source>
      <commands EXPORT="discard">make example-broadcast.sky TARGET=sky</commands>
      <firmware EXPORT="copy">[CONTIKI_DIR]/examples/rime/example-broadcast.sky</firmware>
      <moteinterface>se.sics.cooja.interfaces.Position</moteinterface>
      <moteinterface>se.sics.cooja.interfaces.RimeAddress</moteinterface>
      <moteinterface>se.sics.cooja.interfaces.IPAddress</moteinterface>
      <moteinterface>se.sics.cooja.interfaces.Mote2MoteRelations</moteinterface>
      <moteinterface>se.sics.cooja.interfaces.MoteAttributes</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.MspClock</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.MspMoteID</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.SkyButton</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.SkyFlash</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.SkyCoffeeFilesystem</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.Msp802154Radio</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.MspSerial</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.SkyLED</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.MspDebugOutput</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.SkyTemperature</moteinterface>
    </motetype>
    <mote>
      <breakpoints />
      <interface_config>
        se.sics.cooja.interfaces.Position
        <x>50.0</x>
        <y>0.0</y>
        <z>0.0</z>
      </interface_config>
      <interface_config>
        se.sics.cooja.mspmote.interfaces.MspMoteID
        <id>1</id>
      </interface_config>
      <motetype_identifier>sky1</motetype_identifier>
    </mote>
    <mote>
      <breakpoints />
      <interface_config>
        se.sics.cooja.interfaces.Position
        <x>90.0</x>
        <y>0.0</y>
        <z>0.0</z>
      </interface_config>
      <interface_config>
        se.sics.cooja.mspmote.interfaces.MspMoteID
        <id>2</id>
      </interface_config>
      <motetype_identifier>sky1</motetype_identifier>
    </mote>
    <mote>
      <breakpoints />
      <interface_config>
        se.sics.cooja.interfaces.Position
        <x>130.0</x>
        <y>0.0</y>
        <z>0.0</z>
      </interface_config>
      <interface_config>
        se.sics.cooja.mspmote.interfaces.MspMoteID
        <id>3</id>
      </interface_config>
      <motetype_identifier>sky1</motetype_identifier>
    </mote>
    <mote>
      <breakpoints />
      <interface_config>
        se.sics.cooja.interfaces.Position
        <x>130.0</x>
        <y>40.0</y>
        <z>0.0</z>
      </interface_config>
      <interface_config>
        se.sics.cooja.mspmote.interfaces.MspMoteID
        <id>4</id>
      </interface_config>
      <motetype_identifier>sky1</motetype_identifier>
    </mote>
    <mote>
      <breakpoints />
      <interface_config>
        se.sics.cooja.interfaces.Position
        <x>130.0</x>
        <y>-40.0</y>
        <z>0.0</z>
      </interface_config>
      <interface_config>
        se.sics.cooja.mspmote.interfaces.MspMoteID
        <id>5</id>
      </interface_config>
      <motetype_identifier>sky1</motetype_identifier>
    </mote>
  </simulation>
  <plugin>
    se.sics.cooja.plugins.SimControl
    <width>280</width>
    <z>1</z>
    <height>160</height>
    <location_x>400</location_x>
    <location_y>0</location_y>
  </plugin>
  <plugin>
    se.sics.cooja.plugins.Visualizer
    <plugin_config>
      <moterelations>true</moterelations>
      <skin>se.sics.cooja.plugins.skins.IDVisualizerSkin</skin>
      <skin>se.sics.cooja.plugins.skins.TrafficVisualizerSkin</skin>
      <skin>se.sics.cooja.plugins.skins.LogVisualizerSkin</skin>
      <viewport>3.931818181818181 0.0 0.0 3.931818181818181 -159.86363636363635 173.0</viewport>
    </plugin_config>
    <width>400</width>
    <z>2</z>
    <height>400</height>
    <location_x>1</location_x>
    <location_y>1</location_y>
  </plugin>
  <plugin>
    se.sics.cooja.plugins.LogListener
    <plugin_config>
      <filter />
      <formatted_time />
      <coloring />
    </plugin_config>
    <width>1520</width>
    <z>3</z>
    <height>240</height>
    <location_x>400</location_x>
    <location_y>160</location_y>
  </plugin>
  <plugin>
    se.sics.cooja.plugins.TimeLine
    <plugin_config>
      <mote>0</mote>
      <mote>1</mote>
      <mote>2</mote>
      <mote>3</mote>
      <mote>4</mote>
      <showRadioRXTX />
      <showRadioHW />
      <showLEDs />
      <zoomfactor>500.0</zoomfactor>
    </plugin_config>
    <width>1920</width>
    <z>5</z>
    <height>166</height>
    <location_x>0</location_x>
    <location_y>718</location_y>
  </plugin>
  <plugin>
    se.sics.cooja.plugins.Notes
    <plugin_config>
      <notes>Enter notes here</notes>
      <decorations>true</decorations>
    </plugin_config>
    <width>1240</width>
    <z>4</z>
    <height>160</height>
    <location_x>680</location_x>
    <location_y>0</location_y>
  </plugin>
  <plugin>
    se.sics.cooja.plugins.ScriptRunner
    <plugin_config>
      <script>/*&#xD;
 * Author: Marvin Haus&#xD;
 * &#xD;
 * Testfile that checks the correct working of the&#xD;
 * k-hop algorithm in the 01-2hop-test.csc&#xD;
 * Nodes have to send messages in the form of&#xD;
 * "Testcase:Node:id,hop,posX,posY"&#xD;
 * "Testcase:Edge:id,id"&#xD;
 * after 21 seconds passed&#xD;
 * &#xD;
 * Open 01-2hop-test.csc in a text editor and change&#xD;
 * &lt;source EXPORT="discard"&gt;[CONTIKI_DIR]/examples/rime/example-broadcast.c&lt;/source&gt;&#xD;
 * &lt;commands EXPORT="discard"&gt;make example-broadcast.sky TARGET=sky&lt;/commands&gt;&#xD;
 * &lt;firmware EXPORT="copy"&gt;[CONTIKI_DIR]/examples/rime/example-broadcast.sky&lt;/firmware&gt;&#xD;
 * to the path of the correct file&#xD;
 */&#xD;
&#xD;
/* Make test automatically fail (timeout) after 120 simulated seconds */&#xD;
TIMEOUT(120000); /* milliseconds. no action at timeout */&#xD;
&#xD;
/* Some variables */&#xD;
moteDict = {};          //Associative Array that will hold messages of every node&#xD;
moteAmount = 0;         //Amount of motes in the Simulation&#xD;
testSuccess = false;    //Set to true if the testcondition is fulfilled&#xD;
allmotes = [];&#xD;
&#xD;
/* Check if the element value is contained in the array */&#xD;
function arrayContains(array, value) {&#xD;
  return array.indexOf(value) &gt; -1;&#xD;
}&#xD;
&#xD;
/* Calculate distance between 2 nodes with the IDs specified */&#xD;
function calcDistance(id1, id2) {&#xD;
    for(var i = 0; i &lt; moteAmount; i++){&#xD;
        if(allmotes[i].getID() == id1){&#xD;
            mote1 = allmotes[i];&#xD;
           }&#xD;
        if(allmotes[i].getID() == id2){&#xD;
            mote2 = allmotes[i];&#xD;
           }&#xD;
	}&#xD;
    return mote1.getInterfaces().getPosition().getDistanceTo(mote2);&#xD;
}&#xD;
&#xD;
/* Wait until node has booted */&#xD;
WAIT_UNTIL(msg.startsWith('Starting'));&#xD;
&#xD;
/* Fill the dictionary with the ID of every mote as keys and an empty Array as value*/&#xD;
 allmotes = sim.getMotes();&#xD;
 moteAmount = allmotes.length;&#xD;
 for(var i = 0; i &lt; moteAmount; i++){&#xD;
      moteDict[allmotes[i].getID()] = [];&#xD;
 }&#xD;
 &#xD;
/* Wait 20 seconds (20000ms) to build network */&#xD;
GENERATE_MSG(20000, "continue");&#xD;
YIELD_THEN_WAIT_UNTIL(msg.equals("continue"));&#xD;
&#xD;
/* wait for building dict */&#xD;
GENERATE_MSG(20000, "endloop");&#xD;
&#xD;
/* Read Testcase messages */&#xD;
while (!msg.equals("endloop")) {&#xD;
  YIELD();&#xD;
  if(msg.substr(0,9).equals("Testcase:")){&#xD;
    moteDict[id].push(msg.substr(10,msg.length()-9));&#xD;
  }&#xD;
}&#xD;
&#xD;
/* Check if test conditions are fulfilled */&#xD;
&#xD;
testSuccess = &#xD;
    /* Node 1 */&#xD;
    arrayContains(moteDict[1], "Node:1,0,50,0") &amp;&amp;&#xD;
    arrayContains(moteDict[1], "Edge:1,2") &amp;&amp;&#xD;
    arrayContains(moteDict[1], "Node:2,1,90,0") &amp;&amp;&#xD;
    arrayContains(moteDict[1], "Edge:2,3") &amp;&amp;&#xD;
    arrayContains(moteDict[1], "Edge:2,1") &amp;&amp;&#xD;
    arrayContains(moteDict[1], "Node:3,2,130,0") &amp;&amp;&#xD;
    /* Node 2 */&#xD;
    arrayContains(moteDict[2], "Node:2,0,90,0") &amp;&amp;&#xD;
    arrayContains(moteDict[2], "Edge:2,1") &amp;&amp;&#xD;
    arrayContains(moteDict[2], "Edge:2,3") &amp;&amp;&#xD;
    arrayContains(moteDict[2], "Node:1,1,50,0") &amp;&amp;&#xD;
    arrayContains(moteDict[2], "Edge:1,2") &amp;&amp;&#xD;
    arrayContains(moteDict[2], "Node:3,1,130,0") &amp;&amp;&#xD;
    arrayContains(moteDict[2], "Edge:3,2") &amp;&amp;&#xD;
    arrayContains(moteDict[2], "Edge:3,4") &amp;&amp;&#xD;
    arrayContains(moteDict[2], "Edge:3,5") &amp;&amp;&#xD;
    arrayContains(moteDict[2], "Node:4,2,130,40") &amp;&amp;&#xD;
    arrayContains(moteDict[2], "Node:5,2,130,-40") &amp;&amp;&#xD;
    /* Node 3 */&#xD;
    arrayContains(moteDict[3], "Node:3,0,130,0") &amp;&amp;&#xD;
    arrayContains(moteDict[3], "Edge:3,2") &amp;&amp;&#xD;
    arrayContains(moteDict[3], "Edge:3,4") &amp;&amp;&#xD;
    arrayContains(moteDict[3], "Edge:3,5") &amp;&amp;&#xD;
    arrayContains(moteDict[3], "Node:2,1,90,0") &amp;&amp;&#xD;
    arrayContains(moteDict[3], "Edge:2,1") &amp;&amp;&#xD;
    arrayContains(moteDict[3], "Node:4,1,130,40") &amp;&amp;&#xD;
    arrayContains(moteDict[3], "Edge:4,3") &amp;&amp;&#xD;
    arrayContains(moteDict[3], "Node:5,1,130,-40") &amp;&amp;&#xD;
    arrayContains(moteDict[3], "Edge:5,3") &amp;&amp;&#xD;
    arrayContains(moteDict[3], "Node:1,2,50,0") &amp;&amp;&#xD;
    /* Node 4 */&#xD;
    arrayContains(moteDict[4], "Node:4,0,130,40") &amp;&amp;&#xD;
    arrayContains(moteDict[4], "Edge:4,3") &amp;&amp;&#xD;
    arrayContains(moteDict[4], "Node:3,1,130,0") &amp;&amp;&#xD;
    arrayContains(moteDict[4], "Edge:3,2") &amp;&amp;&#xD;
    arrayContains(moteDict[4], "Edge:3,4") &amp;&amp;&#xD;
    arrayContains(moteDict[4], "Edge:3,5") &amp;&amp;&#xD;
    arrayContains(moteDict[4], "Node:2,2,90,0") &amp;&amp;&#xD;
    arrayContains(moteDict[4], "Node:5,2,130,-40") &amp;&amp;&#xD;
    /* Node 5 */&#xD;
    arrayContains(moteDict[5], "Node:5,0,130,-40") &amp;&amp;&#xD;
    arrayContains(moteDict[5], "Edge:5,3") &amp;&amp;&#xD;
    arrayContains(moteDict[5], "Node:3,1,130,0") &amp;&amp;&#xD;
    arrayContains(moteDict[5], "Edge:3,2") &amp;&amp;&#xD;
    arrayContains(moteDict[5], "Edge:3,4") &amp;&amp;&#xD;
    arrayContains(moteDict[5], "Edge:3,5") &amp;&amp;&#xD;
    arrayContains(moteDict[5], "Node:2,2,90,0") &amp;&amp;&#xD;
    arrayContains(moteDict[5], "Node:4,2,130,40")&#xD;
    ;&#xD;
&#xD;
/* iterate nodes */&#xD;
//for(var i = 1; i &lt;= moteAmount; i++){&#xD;
//    /* iterate messages */&#xD;
//    for(var j = 0; j &lt; moteDict[i].length; j++){&#xD;
//        /* iterate other nodes */&#xD;
//        for(var k = 1; k &lt;= moteAmount; k++){&#xD;
//            /* check if the other mode is a neighbor (transmission range is 50) */&#xD;
//            if(i != k &amp;&amp; calcDistance(i,k) &lt; 50){&#xD;
//                   arrayContains(moteDict[k], moteDict[i][j]);&#xD;
//                }&#xD;
//        }&#xD;
//    }&#xD;
// }&#xD;
/* Was the test a success?*/&#xD;
&#xD;
if(testSuccess){&#xD;
    log.testOK();&#xD;
}&#xD;
&#xD;
/* print */&#xD;
for(var i = 1; i &lt;= moteAmount; i++){&#xD;
    for(var j = 0; j &lt; moteDict[i].length; j++){&#xD;
      log.log(moteDict[i][j] + "\n");&#xD;
      }&#xD;
}</script>
      <active>true</active>
    </plugin_config>
    <width>600</width>
    <z>0</z>
    <height>700</height>
    <location_x>831</location_x>
    <location_y>57</location_y>
  </plugin>
</simconf>

