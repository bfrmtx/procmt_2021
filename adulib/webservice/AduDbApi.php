<?php

/**
 * this contants define the single event types.
 */
define("C_EVENT_INDEX_NONE",                        0);
define("C_EVENT_INDEX_ACTIVATE_SLEEP_MODE",         1);
define("C_EVENT_INDEX_DEACTIVATE_SLEEP_MODE",       2);
define("C_EVENT_INDEX_E_MAIL_EVENT",                3);
define("C_EVENT_INDEX_RESET_SLEEP_MODE",            4);
define("C_EVENT_INDEX_START_MOVING_MT",             5);
define("C_EVENT_INDEX_STOP_MOVING_MT",              6);
define("C_EVENT_INDEX_SEND_STATUS_SMS",             7);
define("C_EVENT_INDEX_AUTOMATIC_STATUS_SMS",        8);
define("C_EVENT_INDEX_REQUEST_SENSOR_DETECTION",    9);
define("C_EVENT_INDEX_RESET_GPS_WARM",             10);
define("C_EVENT_INDEX_RESET_GPS_COLD",             11);
define("C_EVENT_INDEX_ACTIVATE_WLAN",              12);
define("C_EVENT_INDEX_DEACTIVATE_WLAN",            13);
define("C_EVENT_INDEX_TOGGLE_GPS_DYNA_MODE",       14);
define("C_EVENT_INDEX_EXECUTE_SHELL_SCRIPT",       15);
define("C_EVENT_INDEX_WRITE_CEA_LOG_FILE",         16);
define("C_EVENT_INDEX_START_COPY_TS_TO_USB",       17);
define("C_EVENT_INDEX_EJECT_SD",       		       18);
define("C_EVENT_INDEX_INSERT_SD",       	       19);
define("C_EVENT_INDEX_CLEAR_FORMAT_SD",       	   20);
define("C_EVENT_INDEX_REQUEST_RESTART_SENSOR_TEST",21);

require_once '/var/www/awi/adu/adu_joblists.php';
require_once '/var/www/awi/adu/adu_db.php';



class AduDbApi {
    private $db;
    private $dbAbstraction;
   
    /**
     * Builds a new Database object.
     *
     * @param dbAbstraction Determines the database abstraction library to use (Default: 'PDO', 'mysqli')
     *                      If an unsupported value is given we default back to 'PDO'.
     */
	function __construct($dbAbstraction = 'PDO') {
        $servername = '127.0.0.1';
        $port = 3306;
        $dbname = 'mcpdb'; //mcpdb7
        $password = 'neptun';
        $username = 'aduuser';
        
        if ($dbAbstraction === 'mysqli') {
            $this->db = new mysqli($servername, $username, $password, $dbname, $port);
            $this->db->autocommit(FALSE);
        } else
        {        
	    try {
		$this->db = new PDO("mysql:host=$servername;port=$port;dbname=$dbname", $username, $password, array(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION, PDO::ATTR_PERSISTENT => true));
		// set the PDO error mode to exception
		//$this->db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
	    } catch (PDOException $e) {
		echo "Connection failed: " . $e->getMessage();
	    }
	}
        
        $this->dbAbstraction = $dbAbstraction;
    }
 
    /**
     * Closes the databse connection upon destruction.
     */
    function __destruct() {
        if ($this->dbAbstraction === 'mysqli') {
            $this->db->close();
        } else {
            $this->db = null;
        }
    }

    /**
     * explicitely close database connection via function call
     */
    function closeDB() {
        if ($this->dbAbstraction === 'mysqli') {
            $this->db->close();
        } else {
	    unset ($this->db);
        }
    }    
    
    /**
     * Outputs the current HwConfig as xml.
     */
    function getHwConfig() {
        $xmlData = $this->checkedSystemDataFetch('HwConfig');
        $this->answerXmlData($xmlData);
    }
    
    /**
     * Outputs the current HwDatabase as xml.
     */
    function getHwDatabase() {
        $xmlData = $this->checkedSystemDataFetch('HwDatabase');
        $this->answerXmlData($xmlData);
    }
    
    /**
     * Outputs the current HwStatus as xml.
     */
    function getHwStatus() {
        $xmlData = $this->checkedSystemDataFetch('HwStatus');
        $this->answerXmlData($xmlData);
    }
    
     function getChannelConfig() {
        $xmlData = $this->checkedSystemDataFetch('ChannelConfig');
        $this->answerXmlData($xmlData);
    }
    
    /**
     * Outputs the current Selftest as xml.
     */
    function getSelftest() {
        $xmlData = $this->checkedSystemDataFetch('Selftest');
        $this->answerXmlData($xmlData);
    }

    /**
     * Outputs the OpenMT meas-doc
     */
    function getOpenMTMeasDoc() {
        $xmlData = $this->checkedOpenMTDataFetch('meas_doc_xml');
        $this->answerXmlData($xmlData);
    }
    
    /**
     * Output a list of job descriptors
     * <jobdescriptors>
     *   <jobdescriptor id='1' isMeasurement='true' start='2016-09-05 14:00:00' stop='2016-09-05 14:30:00' freq='512'>
     *   ...
     * </jobdescriptors>
     */
    function getJobs() {
        $table = $this->checkedJobTableFetch();
        $xmlData = $this->buildDescriptorList($table);
        $this->answerXmlData($xmlData);
    }

    /**
     * Output a list with names of all joblists stored on ADU
     * <joblistdescriptors>
     *   <joblistdescriptor name='<joblist name>'>
     *   ...
     * </joblistdescriptors>
     */
    function getJobListNames() {
        $table   = $this->checkedJobListNameTableFetch();
        $xmlData = $this->buildJobListNameDescriptorList($table);
        $this->answerXmlData($xmlData);
    }
    
    /**
     * Outputs a list of job descriptors for jobs that lie in the future
     */
    function getFutureJobs() {
        $table = $this->checkedJobTableFetch(date('Y-m-d H:i:s', time()));
        $xmlData = $this->buildDescriptorList($table);
        $this->answerXmlData($xmlData);
    }
    
    /**
     * Outputs the xml data for the job with the given Id.
     */
    function getJob($jobid) {
        // TODO : refactor
        if ($this->dbAbstraction === 'mysqli') {
            $stmt = $this->db->prepare('SELECT job FROM jobs WHERE id=' . $jobid);
            $stmt->execute();
            $stmt->bind_result($job);
            $result = "";
            if ($stmt->fetch()) {
                $result = $job;
            }
            $stmt->close();
            header('Content-type: text/xml');
            http_response_code(200);
            echo $result;
        } else {
            $stmt = $this->db->prepare('SELECT job FROM jobs WHERE id=' . $jobid);
            $stmt->execute();
            $result = $stmt->fetch(PDO::FETCH_ASSOC);
            if ($result) {
                $result = $result['job'];
            }
            $stmt->closeCursor();
            #$stmt = null;
            unset ($stmt);
            header('Content-type: text/xml');
            http_response_code(200);
            echo $result;
        }
    }
    
    /**
     * Adds a job to the table 'jobs'.
     */
    function postJob() {
        $jobxml = file_get_contents('php://input');
        $startTime = "";
        $stopTime = "";
        $this->parseStartAndStopTime($jobxml, $startTime, $stopTime);
        
        // TODO : refactor
        if ($this->dbAbstraction === 'mysqli') {
            $stmt = $this->db->prepare('INSERT INTO job(start, stop, job) VALUES(?,?,?)');
            $stmt->bind_param('sss', $startTime, $stopTime, $jobxml);
            $stmt->execute();
            $stmt->close();
            header('Content-type: text/xml');
            http_response_code(200);
            echo "";
            echo $jobxml;
        } else {
            $stmt = $this->db->prepare('INSERT INTO jobs(start, stop, job) VALUES(:startTime, :stopTime, :jobXml)');
            $stmt->bindParam(':startTime', $startTime);
            $stmt->bindParam(':stopTime', $stopTime);
            $stmt->bindParam(':jobXml', $jobxml);
            $stmt->execute();
            $stmt->closeCursor();
            #$stmt = null;
            unset ($stmt);
            header('Content-type: text/xml');
            http_response_code(200);
            echo "";
            echo $jobxml;
        }
    }

    
    /**
     * Adds a job to the table 'jobs'.
     */
    function postStartJoblist() {
        $dataXml = file_get_contents('php://input');
        
        $measurement = new SimpleXMLElement($dataXml);
        
        echo "\n\n==> starting joblist:\nname: \"" . $measurement->name . "\"\nstart-date: \"" . $measurement->startdate . "\"\nstart-time: \"" . $measurement->starttime . "\"\n";        
        
        $aduJoblist = new AduJoblists      ("127.0.0.1");
        $aduJoblist->selectJoblist         ($measurement->name);        
        $aduJoblist->setTimeFormat         ("adjusted");
        $aduJoblist->setStartTime          (strtotime($measurement->startdate . " " . $measurement->starttime));
        $aduJoblist->setCleanJobTable      (TRUE);
        $aduJoblist->setAdaptChannelConfig (TRUE);
        $aduJoblist->setAdaptConfig        (TRUE);
        $aduJoblist->setAdaptSensorSpacing (TRUE);
        $aduJoblist->setAdaptSensorConfig  ("ALL");        
        $aduJoblist->startJobList          ();
        
        unset ($aduJoblist);        
        
        echo "done!\n";
    }
    
    
    /**
     * Stops the first running job.
     */
    function postStopJob() {
        $localMeasIndexes = '0';
        date_default_timezone_set('UTC');
        $localStartTime = date('Y-m-d H:i:s', time() + 10);
        $localStopTime = date('Y-m-d H:i:s', time() + 310);
        
        $stopJobXml = new SimpleXMLElement('<?xml version="1.0" encoding="utf-8" standalone="no" ?><stop_measurement></stop_measurement>');
        $measIndexesXml = $stopJobXml->addChild('meas_indexes', $localMeasIndexes);
        $localJobXml = $stopJobXml->asXML();
    
        // TODO : refactor
        if ($this->dbAbstraction === 'mysqli') {
            $stmt = $this->db->prepare('INSERT INTO job(start, stop, job) VALUES(?,?,?)');
            $stmt->bind_param('sss', $localStartTime, $localStopTime, $localJobXml);
            $stmt->execute();
            $stmt->close();
            header('Content-type: text/xml');
            http_response_code(200);
            echo "true";
        } else {
            $stmt = $this->db->prepare('INSERT INTO jobs(start, stop, job) VALUES(:startTime, :stopTime, :jobXml)');
            $stmt->bindParam(':startTime', $localStartTime);
            $stmt->bindParam(':stopTime', $localStopTime);
            $stmt->bindParam(':jobXml', $localJobXml);
            $stmt->execute();
            $stmt->closeCursor();
            #$stmt = null;
            unset ($stmt);
            header('Content-type: text/xml');
            http_response_code(200);
            echo "true";
        }
    }

	
    function postEventJob ($strEventType, $strEventParam)
	{	
        date_default_timezone_set('UTC');
        $localStartDateTime = date('Y-m-d H:i:s', time() + 10);
        $localStopDateTime  = date('Y-m-d H:i:s', time() + 310);

        $localStartTime = date('H:i:s', time() + 10);
        $localStartDate = date('Y-m-d', time() + 10);
        $localStopTime  = date('H:i:s', time() + 310);
        $localStopDate  = date('Y-m-d', time() + 310);
        
        $tempJobXml     = new SimpleXMLElement ('<?xml version="1.0" encoding="utf-8" standalone="no" ?><event><recording><start_time>' . $localStartTime . '</start_time><start_date>' . $localStartDate . '</start_date><stop_time>' . $localStopTime . '</stop_time><stop_date>' . $localStopDate . '</stop_date><CyclicEvent>0</CyclicEvent><Cycle>0</Cycle><Granularity>seconds</Granularity></recording></event>');
        $measIndexesXml = $tempJobXml->addChild('EventType',      $strEventType);
        $measIndexesXml = $tempJobXml->addChild('EventParameter', $strEventParam);
        $localJobXml    = $tempJobXml->asXML();
    
        // TODO : refactor
        if ($this->dbAbstraction === 'mysqli') {
            $stmt = $this->db->prepare('INSERT INTO job(start, stop, job) VALUES(?,?,?)');
            $stmt->bind_param('sss', $localStartTime, $localStopTime, $localJobXml);
            $stmt->execute();
            $stmt->close();
            header('Content-type: text/xml');
            http_response_code(200);
            echo "true";
        } else {
            $stmt = $this->db->prepare('INSERT INTO jobs(start, stop, job) VALUES(:startTime, :stopTime, :jobXml)');
            $stmt->bindParam(':startTime', $localStartDateTime);
            $stmt->bindParam(':stopTime',  $localStopDateTime);
            $stmt->bindParam(':jobXml',    $localJobXml);
            $stmt->execute();
            $stmt->closeCursor();
            #$stmt = null;
            unset ($stmt);
            header('Content-type: text/xml');
            http_response_code(200);
            echo "true";
        }
    }
	
    /**
     * Start Copy TS To USB pocess.
     */
    function postStartCopyTSToUSB()
	{
		$this->postEventJob ((string)C_EVENT_INDEX_START_COPY_TS_TO_USB, "");
	}

	
    /**
     * Eject TS Data SD-Card
     */
    function postEjectTSDataSDCard()
	{
		$this->postEventJob ((string)C_EVENT_INDEX_EJECT_SD, "");
	}

	
    /**
     * Insert TS Data SD-Card
     */
    function postInsertTSDataSDCard()
	{
		$this->postEventJob ((string)C_EVENT_INDEX_INSERT_SD, "");
	}

    /**
     * Toggle GPS Dynamic Mode
     */
    function postToggleGPSDynaMode()
	{
		$this->postEventJob ((string)C_EVENT_INDEX_TOGGLE_GPS_DYNA_MODE, "");
	}
	
    /**
     * Format TS Data SD-Card
     */
    function postFormatTSDataSDCard()
	{
		$this->postEventJob ((string)C_EVENT_INDEX_CLEAR_FORMAT_SD, "");
	}

    /**
     * Request Sensor Detection
     */
    function postRequestSensorDetection()
	{
		$this->postEventJob ((string)C_EVENT_INDEX_REQUEST_SENSOR_DETECTION, "");
	}
	
    /**
     * Restart Sensor Test selftests to get new values for AutoGains and Probe Resistance
     */
    function postRestartSensorTest()
	{
		$this->postEventJob ((string)C_EVENT_INDEX_REQUEST_RESTART_SENSOR_TEST, "");
	}
	
    function postChannelConfig() {
        $channelConfigXml = file_get_contents('php://input');
        $this->checkedSystemDataUpdate('ChannelConfig', $channelConfigXml);
        header('Content-type: text/xml');
        http_response_code(200);
        echo "true";
    }
    
    /**
     * Convenience function for system table.
     * Fetches an entry from the system table and renders it as XML to the browser.
     */
    private function checkedSystemDataFetch($entryValue) {
        // TODO: Error checking
        if ($this->dbAbstraction === 'mysqli') {
            $stmt = $this->db->prepare('SELECT xml_data FROM system WHERE entry="' . $entryValue . '"');
            $stmt->execute();
            $stmt->bind_result($xml_data);
            $result = "";
            if ($stmt->fetch()) {
                $result = $xml_data;
            }
            $stmt->close();
            return $result;
        } else {        
            $stmt = $this->db->prepare('SELECT xml_data FROM system WHERE entry="' . $entryValue . '"');
            $stmt->execute();
            $result = $stmt->fetch(PDO::FETCH_ASSOC);
            if ($result) {
                $result = $result['xml_data'];
            }
            $stmt->closeCursor();
            #$stmt = null;
            unset ($stmt);
            return $result;
        }
    }
    
    private function checkedSystemDataUpdate($entryValue, $xml_data) {
        if ($this->dbAbstraction === 'mysqli') {
            $stmt = $this->db->prepare('UPDATE system SET xml_data = ? WHERE entry="' . $entryValue . '"');
            $stmt->bind_param('s', $xml_data);
            $stmt->execute();
            $stmt->close();
        } else {        
            $stmt = $this->db->prepare('UPDATE system SET xml_data = :value WHERE entry="' . $entryValue . '"');
            $stmt->bindParam(':value', $xml_data);
            $stmt->execute();
            $stmt->closeCursor();
            #$stmt = null;
            unset ($stmt);
        }
    }

    private function checkedOpenMTDataFetch($entryValue) {
        // TODO: Error checking
        if ($this->dbAbstraction === 'mysqli') {
            $stmt = $this->db->prepare('SELECT ' . $entryValue . ' FROM OpenMT WHERE 1 LIMIT 1');
            $stmt->execute();
            $stmt->bind_result($xml_data);

	    echo "===> <br>";
            
            $result = "";
            if ($stmt->fetch()) {
                $result = $xml_data;
            }
            $stmt->close();
            return $result;
        } else {        
            $stmt = $this->db->prepare('SELECT ' . $entryValue . ' FROM OpenMT WHERE 1 LIMIT 1');
            $stmt->execute();
            $result = $stmt->fetch(PDO::FETCH_ASSOC);
            if ($result) {
                $result = $result[$entryValue];
            }
            $stmt->closeCursor();
            #$stmt = null;
            unset ($stmt);
            return $result;
        }
    }
    
    /**
     * Loads the table 'jobs' into a 2D array and returns it.
     * Constrains the result set to jobs that start after @minStart. By default this value is "1970-01-01 00:00:00".
     * @pre The DbApi is connected to a database containing a job table.
     * @post result.size = jobtable.count
     * [['id' => 1, 'start' => '2016-09-05 08:15:30', 'stop' => '2016-09-05 09:15:30', 'job' => '<measurement> ... </measurement>'], ...]
     */
    private function checkedJobTableFetch($minStart = "1970-01-01 00:00:00") {
        $table = [];
        if ($this->dbAbstraction === 'mysqli') {
            $stmt = $this->db->prepare('SELECT id, start, stop, job FROM jobs WHERE start > ?');
            $stmt->bind_param('s', $minStart);
            $stmt->execute();
            $sqlResult = $stmt->get_result();
            $table = $sqlResult->fetch_all(MYSQLI_ASSOC);
            $stmt->close();
        } else {
            $stmt = $this->db->prepare('SELECT id, start, stop, job FROM jobs WHERE start > :minStart');
            $stmt->bindParam(':minStart', $minStart);
            $stmt->execute();
            $table = $stmt->fetchAll(PDO::FETCH_ASSOC);
            $stmt->closeCursor();
            #$stmt = null;
            unset ($stmt);
        }
        return $table;
    }

    /**
     * Loads the table 'joblists' into a 2D array and returns it.
     * @pre The DbApi is connected to a database containing a job table.
     * @post result.size = jobliststable.count
     */
    private function checkedJobListNameTableFetch() {
        $table = [];
        if ($this->dbAbstraction === 'mysqli') {
            $stmt = $this->db->prepare('SELECT listname FROM joblists WHERE jobname = "ADUConf"');
            $stmt->execute();
            $sqlResult = $stmt->get_result();
            $table = $sqlResult->fetch_all(MYSQLI_ASSOC);
            $stmt->close();
        } else {
            $stmt = $this->db->prepare('SELECT listname FROM joblists WHERE jobname = "ADUConf"');
            $stmt->execute();
            $table = $stmt->fetchAll(PDO::FETCH_ASSOC);
            $stmt->closeCursor();
            unset ($stmt);
        }
            
        return $table;
    }
    
    /**
     * Builds a xml representation of a job table and returns it.
     * @pre jobTable is an array containing rows with fields 'id', 'start', 'stop', 'job'
     * @post result is a valid xml string
     *
     * <jobdescriptors>
     *  <jobdescriptor id='1' start='2016-09-05 08:15:30' stop='2016-09-05 09:15:30' isMeasurement='true' freq='512'>
     * </jobdescriptors>
     */
    private function buildDescriptorList(&$jobTable) {
        $descriptorsXml = new SimpleXMLElement('<?xml version="1.0" encoding="UTF-8"?><jobdescriptors></jobdescriptors>');
        foreach($jobTable as &$row) {
            $descriptor = $descriptorsXml->addChild('jobdescriptor');
            $jobXml = new SimpleXMLElement($row['job']);
            $isMeasurement = false;
            $freq = '';
            if ($jobXml->getName() === 'measurement') {
                $isMeasurement = true;
                try {
                    $freq = $jobXml->recording->input->Hardware->global_config->sample_freq;
                } catch (Exception $e) {
                    $freq = '';
                }
            }
            $jobXml = null;
            
            $descriptor->addAttribute('id', $row['id']);
            $descriptor->addAttribute('isMeasurement', $isMeasurement);
            $descriptor->addAttribute('start', $row['start']);
            $descriptor->addAttribute('stop', $row['stop']);
            $descriptor->addAttribute('freq', $freq);
        }
        unset($row);
        return $descriptorsXml->asXML();
    }

    /**
     * Builds a xml representation of a joblist names table and returns it.
     * @pre jobListNameTable is an array containing rows with fields 'listname'
     * @post result is a valid xml string
     *
     * <joblistsdescriptors>
     *  <joblistsdescriptor name='<Joblist Name>'>
     * </joblistsdescriptors>
     */
    private function buildJobListNameDescriptorList(&$jobTable) {
        $descriptorsXml = new SimpleXMLElement('<?xml version="1.0" encoding="UTF-8"?><joblistsdescriptors></joblistsdescriptors>');
        foreach($jobTable as &$row) {
            $descriptor = $descriptorsXml->addChild('joblistsdescriptor');
            $descriptor->addAttribute('name', $row['listname']);
        }
        unset($row);
        return $descriptorsXml->asXML();
    }
    
    private function parseStartAndStopTime($jobxml, &$startTimeOut, &$stopTimeOut) {
        $measurement = new SimpleXMLElement($jobxml);
        $startTimeOut = "";
        $startTimeOut = $measurement->recording->start_date;
        $startTimeOut = $startTimeOut . " " . $measurement->recording->start_time;
        $stopTimeOut = "";
        $stopTimeOut = $measurement->recording->stop_date;
        $stopTimeOut = $stopTimeOut . " " . $measurement->recording->stop_time;
    }
    
    private function answerXmlData(&$xmlData) {
        header('Content-type: text/xml');
        http_response_code(200);
        echo $xmlData;
    }
    
       /**
     * Truncates the jobs table.
     *
     */
    function postClearJobs() { // TUT 10: here the job table is truncated, back to adunetwork::onClearJobsFinished
        if ($this->dbAbstraction === 'mysqli') {
            $stmt = $this->db->prepare('TRUNCATE TABLE jobs');
            $stmt->execute();
            $stmt->close();
            header('Content-type: text/xml');
            http_response_code(200);
            echo "true";
        } else {
            $stmt = $this->db->prepare('TRUNCATE TABLE jobs');
            $stmt->execute();
            $stmt->closeCursor();
            #$stmt = null;
            unset ($stmt);
            header('Content-type: text/xml');
            http_response_code(200);
            echo "true";
        }
    }
    
    function postClearFutureJobs() { // TUT 20: Actually I think we wanted to delete all not started jobs when submitting, back to rewiring the submit process
        if ($this->dbAbstraction === 'mysqli') {
            $stmt = $this->db->prepare('DELETE FROM jobs WHERE started = 0');
            $stmt->execute();
            $stmt->close();
            header('Content-type: text/xml');
            http_response_code(200);
            echo "true";
        } else {
            $stmt = $this->db->prepare('DELETE FROM jobs WHERE started = 0');
            $stmt->execute();
            $stmt->closeCursor();
            #$stmt = null;
            unset ($stmt);
            header('Content-type: text/xml');
            http_response_code(200);
            echo "true";
        }
    }
}

?>
