<?php
/*
 * Purpose: to detect any calls to d() "debug" function.
 * 			If the Devel module is installed, the function dsm() is called
 * 			This function ensures we do not receive fatal errors when moving dev code to the live site
 * Category: Troubleshooting
 */ 
function d($first, $second = ''){
  global $user;
  if (function_exists(dsm))
	dsm($first, $second);
}

/******************************************************************************
 * Example 3: Running Mike's code, which detects profile completion, in a module.
 * This will allow us to run it on every page (not just the front page). It will
 * also allow us to avoid the use of the PHP module and its slow EVAL of code.
 */

/**
 * Implementation of hook_init()
 *
 * This hook is run before every non-cached page request in drupal, so code
 * placed in ths hook is guarenteed to run for all logged-in users on every page
 * load. All moduled are loaded when this hook is called.
 * 
 * NOTE: This was one of the first functions we made.
 * 		There is likely a more professional way to accomplish this task, but it works and is lightweight.
 *
 * @global <type> $user
 * 
 * Category: Other
 */
function er_init(){
	// Custom code by Mike to see if user has proper Role indicating they've
	// completed their profile, sends them to their profile page to fill it out
	// if they haven't

	// We need access to the global $user as well as a flag of our own to
	// indicate when we need to add HTML to the page.
	global $user,$er_add_footer;

		//d($user->roles);
	// Loop and find role for completed profile.
		$complete = TRUE;
	if ($user->roles[1] != "anonymous user" && FALSE === array_search("Completed Profile", $user->roles))
			$complete = FALSE;

	// In addition to the checks Mike made, I am adding a check to make sure we
	// are not in the /user section of the site. The arg() function returns the
	// value of the specified url segment, starting with 0.
	if ($complete != TRUE && arg(0)!='users' && arg(0)!='user')
	{
		// By noting this in a global variable, I can check later from my footer
		// hook to see if I need to add the footer HTML to the page.
		$path = drupal_get_path('theme', 'epscor');
				drupal_add_js($path . '/jquery_ui/jquery-ui-1.8.14.custom.min.js');
		//drupal_add_css($path . '/jquery_ui/css/smoothness/jquery-ui-1.8.14.custom.css');
		drupal_add_css($path . '/jquery_ui/css/blue/jquery-ui-1.7.3.custom.css');
				
		$html = '<div id="fillprofile" title="Profile Information Needed">
			<p>Welcome,</p><p>For reporting purposes, a user profile is needed for all members involved.
			You will be directed to your profile now.</p></div>';
	   	//need to remove line breaks or js wont accept it this way =P
		$html = str_replace(array("\n","\r"), '', $html);
		drupal_add_js(
			'jQueryNew(document).ready(function($){
				$(\''.$html.'\').dialog({
					closeOnEscape: false,
					open: function(event, ui) {
						$(".ui-dialog-titlebar-close").hide();
					},
					width: 600,
					modal: true,
					zIndex: 3999,
					buttons: {
						"Ok": function() {
							window.location = "'.url("user/{$user->uid}/profile/er_user_profile").'"
						}
					}
				});
			}).appendTo("body"); '
		,'inline');
	}
	else
	{
		$er_add_footer = false;
	}
}


/* START FORM LOGIC SECTION */

/*
 * Purpose: 	Generic form_alter that will get called whenever a form appears, usually from the Edit tab
 * 				More specific functions like 'er_form_event_node_form_alter' will only get called when the
 * 				form for the content type Event occurs, but this function catches all forms.
 * Category:	Common Hook 
 */
function er_form_alter(&$form, &$form_state, $form_id){
	//Used in the accomplishments views, see collaborations
	//on_dev($form_id, $form, $form_state);
	// d($form_id, 'form id');
	
	dsm($form_id, 'form id');
	echo 'findz';
	
	$check_edit_permission = array('er_book_node_form', 
									'er_proposal_node_form',
									'er_project_node_form',
									'er_publication_node_form',
									'er_nsf_fastlane_node_form',
									'er_presentation_node_form',
									'er_poster_node_form',
									'er_patent_node_form',
									'er_internet_node_form',
									'er_other_product_node_form',
									'ddblock_news_item_node_form',
									'er_event_node_form');
	if (in_array($form_id, $check_edit_permission)){
		// this is one of the content types we check
		//	does this user have permission to edit it? Are they the Creator, Referenced in the Node, or an Admin?
		er_check_edit_permissions($form);
	}
	
	if ($form_id == 'er_user_profile_node_form'){
		// DEV SITE and LIVE act different, live site doesnt seem to want the group_er... part
		if (on_live())
			$form['field_er_teams']['#element_validate'][] = 'user_profile_team_validate';			
		else
			$form['group_er_involvement']['field_er_teams']['#element_validate'][] = 'user_profile_team_validate';
	}	
	
    if ($form_id == 'node_admin_content') {
        if (!empty($form['admin']['status'])) {
            foreach ($form['admin']['status'] as $nid => $title) {
                $updated = db_result(db_query('SELECT changed FROM node WHERE nid = "%d"', $nid));
                $updated_string = date('n/j/y g:ia', $updated);
                $form['admin']['status'][$nid]['#value'] .= '<br/><span class="smaller">(edited '.$updated_string.')</span>';
                //d($nid, 'nid');
            }
        }
    }	
	
	if ($form_id == 'er_user_event_instance_node_form'){
		$form['#after_build'][] = 'ue_after_build_inst';
		
	}

	if ($form_id == 'er_user_event_node_form'){
		// d($form, 'right one');
		if (isset($_GET['ueType'])){
			$changeType = $_GET['ueType'];
			$form['field_er_user_event_type']['#default_value'][0]['value'] = $changeType;		
		}		
		
		$form['#after_build'][] = 'ue_after_build';
		$form['field_er_user_event_date']['#element_validate'][] = 'to_date_in_bounds_validate';
	}	 

	// populate array of all content types that have field_er_user_reference
	$haveUR = array('er_book_node_form', 'er_event_node_form', 'er_proposal_node_form', 'ddblock_news_item_node_form', 'er_internet_node_form', 'er_nsf_fastlane_node_form', 'er_other_product_node_form', 'er_patent_node_form', 'er_poster_node_form', 'er_presentation_node_form', 'er_publication_node_form', 'er_project_node_form');
	if (in_array($form_id, $haveUR)){
			//d($form, 'form');
			//d($form_id, 'form id');
			//d(arg(), 'arg');
			global $user;		
			$node = node_load(arg(1));
			
			//d($node, 'node');
			$ref = $node->field_er_user_reference;
			$refArray = array();
			if(is_array($ref)){
				foreach ($ref as $k){
					$refArray[] = $k['uid'];
				}				
			}
			// the below is deprecated but may be useful sometime			
			// if (!in_array($user->uid, $refArray) && !user_access('administer nodes')){
				// not allowed here
				//drupal_access_denied();
			// }
	}
	
	
	//TODO::THIS WILL SOON BE DEPRECATED
	$allow_multiple = array('er_proposal_node_form','er_collaboration_node_form','er_user_event_node_form');
	//if(isset($form['field_er_reporting_year']) && !in_array($form_id, $allow_multiple)){
	//	$form['#field_info']['field_er_reporting_year']['multiple'] = 0;
	//}
	
	if ($form_id == 'er_user_event_node_form')
		$form['#field_info']['field_er_user_reference']['multiple'] = 0; //THIS ISN'T WORKING... the value gets set but has no effect.
	
	//d($form['#field_info']);
	// This function does not return anything, because the $form variable was
	// passed in by reference, so any changes we make will be
	

	
	//added by Andrew; this is used on accomplishments views, it ensures that if a start date is provided, a to date must also be provided.
	if ($form_id == 'views_exposed_form' && $form['startdate'] && $form['enddate'])
		$form['#validate'][] = '_accomplishments_form_validation';
} // end er_form_alter

/******************************************************************************
  * Example 1: Altering a CCK form field, such as the date repetition interval.
  *
  * This process is more complex than I made it seem in my initial email, due
  * to the fact that CCK fields are built through a move complicated process
  * than normal form fields. It is still possible to alter them, but instead
  * of making our changes in the hook_form_alter() call, we need to hook into
  * the process later, once CCK has had a chance to do its extra processing.
  *
  * The way to do this is to add an "after_build" function to the form, which
  * will be called after CCK is done.
  */


/**
 * This is our hook_form_alter. We don't actually make any changes to the form
 * here, because the fields we want to change have not been added to the form
 * yet. Instead, we will just tell the form process that we want another chance
 * to modify the form once everything else has completed.
 *
 * (If we were modifying a normal form, instead of a CCK form, our changes could
 * be made directly in this function.)
 *
 * @param <array> $form
 * @param <array> $form_state
 * 
 * Category: Form Alter
 */
function er_form_event_node_form_alter(&$form, &$form_state)
{
	// This is the name of the function that will be called once the form build
	// process is complete.
	$function = 'er_alter_repeats';

	// If there are already other modules which have added their own after_build
	// functions, we will simply add ours to the end of the array. If no other
	// module has defined an after_build function yet we will create the array
	// ourselves.
	if(is_array($form['#after_build']))
	{
		$form['#after_build'][] = $function;
	}
	else
	{
		$form['#after_build'] = array($function);
	}

	// This function does not return anything, because the $form variable was
	// passed in by reference, so any changes we make will be done
}

/**
 * This is our after_build function. At this point the form is complete and
 * ready for display, so we can jump in and make our changes.
 *
 * @param <array> $form
 * @param <array> $form_state
 * @return <array> $form
 * 
 * Category: Form Alter
 */
function er_alter_repeats($form, &$form_state)
{
	// Use drupal for firebug here to see the final structure of the form.
	//firep($form);

	// Based on the drupal for firebug output, this is the array path to the
	// form element that contains the list of interval counters. I am going to
	// make a reference to it for readibility of the code. Any changes we make
	// to this reference will carry through to the original array value.
	$element =& $form['field_er_event_date']['rrule']['INTERVAL']['#options'];

	// To modify it I simply use array_slice to pull out the first 12 elements
	// and discard the rest.
	$element = array_slice($element, 0, 12);

	// Now return our modified form array.
	return $form;
}

/*
 * Purpose:	To limit the choices of the Team chosen to just one team. However, a user may be on the MAP or CAC concurrently
 * 			This method of validation is called from er_form_alter on the field element. 
 * Category: Form Validation
 * 
 * @param <array> $element - this is helpful because if it does not validate, you pass this object into the
 * 		form_error function and the result is Drupal highlights the field that needs to be fixed in a red box. 
 */
 
function user_profile_team_validate($element, &$form_state){
	// the line below is needed because validation will mess up new user creation
	if ($element['#post']['form_id'] == 'user_register') return;
	// this makes sure that only ONE team can be selected, but the users can be on multiple committees
	/* HARDCODED */
	$choices = array('CYBER', 'ECOGEM', 'ENDER', 'DEW', 'Administration', 'select_or_other');
	$totalTeams = 0;
	foreach ($choices as $k) {
		$totalTeams += (empty($element['select'][$k]['#value'])) ? 0 : 1;
	}
	if ($totalTeams != 1){
		form_error($element, t('Please select ONE team.'));
	}
}

/*
 * Purpose:	This is used in custom pages dealing with User Events. Look in er/pages, there are a few that add a UID to the 
 * 			$_GET variable. The purpose is to create an easy way for administrators to add User Events to a large batch of 
 * 			users. We used the Lightbox module to open up a Create User Event form, and this function automatically fills
 * 			in the User Reference field according to the UID given in the URL (the $_GET variable)
 * 			
 * 			NOTE: I believe an API call may exist for userreference that accomplishes this, and it may be possible to 
 * 			do the same thing with a simple form_alter where you fill in the UID to the userreference, but this code works.
 * 			It may be done in another way in this code as well.
 * Category: Form Alter
 */
function ue_after_build_inst($form, &$form_state) {	
	// set the UID to be correct if the arg is there for it, wasn't working for some reason
	// NOTE: this is a hack, as this field should be filled in automatically by the userreference module
	//	automatically, I believe. I had to get this working, and this works.
	// TODO: come back and either figure out why this wasn't working, or create a function for the below, so we don't have duplicate lines
	$args = arg();
	// d($args, 'args');
	if (is_numeric($args[3])){
		$ueuid = $args[3];
		$result = db_query('SELECT n.realname, n.uid, u.name FROM {realname} n INNER JOIN {users} u ON n.uid = u.uid WHERE n.uid = '. $ueuid);
  		$user = db_fetch_object($result);
		//d($user, 'user');
		$account = realname_get_user($ueuid);
		//d($account, 'account');
		if ($account->name != NULL) {
			// display Real Name (username)
			$form['field_er_event_inst_user'][0]['uid']['uid']['#value'] = $user->realname . ' ('.$user->name . ')';	
		}else{
			// just display the user name, not realname
			$u = user_load($ueuid);
			//d($u, 'u');
			$form['field_er_event_inst_user'][0]['uid']['uid']['#value'] = $u->name;
		}		
	}		
	
	if (isset($_GET['uid'])){
		$ueuid = $_GET['uid'];
		$result = db_query('SELECT n.realname, n.uid, u.name FROM {realname} n INNER JOIN {users} u ON n.uid = u.uid WHERE n.uid = '. $ueuid);
  		$user = db_fetch_object($result);
		//d($user, 'user');
		$account = realname_get_user($ueuid);
		//d($account, 'account');
		if ($account->name != NULL) {
			// display Real Name (username)
			$form['field_er_event_inst_user'][0]['uid']['uid']['#value'] = $user->realname . ' ('.$user->name . ')';	
		}else{
			// just display the user name, not realname
			$u = user_load($ueuid);
			//d($u, 'u');
			$form['field_er_event_inst_user'][0]['uid']['uid']['#value'] = $u->name;
		}
	}
	return $form;
}

/*
 * Purpose: See notes from ue_after_build_inst, this does the same thing. We have separated the User Events containing
 * 			"From Date" and "To Date" with User Events containing a single date.
 * 			User Event Instance (such as graduation) and User Event
 * Category: Form Alter
 */

function ue_after_build($form, &$form_state) {
	$args = arg();
	// d($args, 'in reg');
	if (isset($_GET['uid'])){
		// d('in if');
		$ueuid = $_GET['uid'];
		$result = db_query('SELECT n.realname, n.uid, u.name FROM {realname} n INNER JOIN {users} u ON n.uid = u.uid WHERE n.uid = '. $ueuid);
  		$user = db_fetch_object($result);
		//d($user, 'user');
		$account = realname_get_user($ueuid);
		//d($account, 'account');
		if ($account->name != NULL) {
			// display Real Name (username)
			$form['field_er_event_user'][0]['uid']['uid']['#value'] = $user->realname . ' ('.$user->name . ')';	
		}else{
			// just display the user name, not realname
			$u = user_load($ueuid);
			//d($u, 'u');
			$form['field_er_event_user'][0]['uid']['uid']['#value'] = $u->name;
		}
	}
	return $form;
}

/*
 * Purpose: This is necessary because of the way the query is being built.
 * 			The query attempts to find any overlap in date ranges, for this to be successful, we need both start and end dates.
 * Category: Form Validation 
 */

function _accomplishments_form_validation(&$form, &$form_state){
	$d1 = $form['startdate']['value']['date']['#value'];
	$d2 = $form['enddate']['value']['date']['#value'];
	if (empty($d1) xor empty($d2)){
		form_set_error('accomplishments', t('If you use choose to view a date range, you must set both the start and end dates.'));
	}
}

/******************************************************************************
 * Example 4: Taking some structured data about the various NSF Fastlans fields
 * and add it to an instructions page on a tab under the create content page.
 *
 * @todo add example to each item.
 *
 * @todo make links in the form to the correct "what" and "why" sections.
 *
 * @todo maybe we can use a popup display like Mike's profile prompt?
 */

/**
 * Just a place to store our default field information. This can (and probably
 * will) be overridden by user-entered values.
 *
 *
 * @return <array>
 */
function er_get_nsf_fastlane_meta_defaults($field = false, $type = false)
{
	$defaults = array(
	'field_er_nsf_activities'=>array(
		'title'=>'Project Activities',
		'example'=>'',
				'what'=>'<p>Please reiterate the goals and objectives of your efforts, and summarize the research and education activities you have engaged in that aim to achieve these objectives. Include experiments you have conducted, the simulations you have run, the collecting you have done, the observations you have made, the materials you have developed, and major presentations you have made about your efforts. In a later section you will list more formally any publications and other specific products (database, collections, software, inventions, etc.) that have resulted.</p>',
		'why'=>'<p>What you tell us under "project activities" and "project findings" will inform your program officer who will be reviewing for satisfactory progress from year to year and then assessing the results from this award once it is completed. What you tell us here also lays the foundation for your subsequent description of specific products (publications, collections, software, etc.) and broader contributions.</p>'
	),
	'field_er_nsf_findings'=>array(
		'title'=>'Project Findings',
		'example'=>'',
		'what'=>'<p>Please summarize the conclusions that have emerged from your activities. Later screens will invite you to identify publications and other concrete products (collections, databases, software, inventions, and so on) and to explain the significance and implications of both findings and products for your field, for other fields, and even beyond science and engineering.</p><p>If you have no findings to report, at least for now, please click the corresponding button. We anticipate that as the project progresses your emphasis in reporting will shift from activities to findings and products, and ultimately to contributions.</p>',
		'why'=>'<p>What you tell us under "project activities" and "project findings" will inform your program officer who will be reviewing for satisfactory progress from year to year and then assessing the results from this award once it is completed. What you tell us here also lays the foundation for your subsequent description of specific products (publications, collections, software, etc.) and broader contributions.</p>'
	),
	'field_er_nsf_training'=>array(
		'title'=>'Training and Development',
		'example'=>'',
		'what'=>'<p>Please summarize the contributions to the research and teaching skills and experience of those who have worked on the project, including undergraduate students, graduate students, post-docs, college faculty, and K-12 teachers. If your project supported postdoctoral researchers then you must include a summary of the mentoring activities conducted.</p>',
		'why'=>'<p>Support for basic research in an education-rich environment characterizes the American research endeavor and distinguishes it from that in many other countries. NSF supports such research in part to attract young people to, and prepare them for, careers in science, mathematics, and engineering. Education in science, math, engineering, and technology for those who will not be making careers in science or technology, plus broad public exposure to those fields, are also part of NSF\'s purpose. Finally, integration of research and education is one of NSF\'s core strategies, for when research and education are effectively integrated, the cost of both is often reduced; and new scientific and engineering knowledge and techniques are more quickly and effectively communicated. NSF therefore needs to learn how those we support are pursuing these ends. Moreover, we evaluate projects in significant part on the basis of results achieved in research training, education and public outreach. So we ask you to describe for those who will be reviewing your project – both the NSF program officer reviewing for satisfactory progress and, later on, reviewers assessing the results from past NSF support – what you have accomplished in these areas. These descriptions also lay a base from which you can later identify, both specific educational products (textbooks, courseware, and so on) that have come out of your project and broader educational contributions that resulted.</p>'
	),
	'field_er_nsf_outreach'=>array(
		'title'=>'Outreach Activities',
		'example'=>'',
		'what'=>'<p>Please summarize any project activities that aimed to reach out to members of communities who are not usually aware of your activities, for the purpose of enhancing participation in science learning and careers in science, public understanding of science and technology, or the like. If you have nothing (yet) to report, please click the corresponding button.</p><p>Later screens will invite you to identify any books or concrete products that have resulted from such activities and to say how the project has contributed beyond its own boundaries to education and development of human resources.</p>',
		'why'=>'<p>Support for basic research in an education-rich environment characterizes the American research endeavor and distinguishes it from that in many other countries. NSF supports such research in part to attract young people to, and prepare them for, careers in science, mathematics, and engineering. Education in science, math, engineering, and technology for those who will not be making careers in science or technology, plus broad public exposure to those fields, are also part of NSF\'s purpose. Finally, integration of research and education is one of NSF\'s core strategies, for when research and education are effectively integrated, the cost of both is often reduced; and new scientific and engineering knowledge and techniques are more quickly and effectively communicated. NSF therefore needs to learn how those we support are pursuing these ends. Moreover, we evaluate projects in significant part on the basis of results achieved in research training, education and public outreach. So we ask you to describe for those who will be reviewing your project – both the NSF program officer reviewing for satisfactory progress and, later on, reviewers assessing the results from past NSF support – what you have accomplished in these areas. These descriptions also lay a base from which you can later identify, both specific educational products (textbooks, courseware, and so on) that have come out of your project and broader educational contributions that resulted.</p>'
	),
	'field_er_nsf_win_discipline'=>array(
		'title'=>'Contributions within Discipline',
		'example'=>'',
		'what'=>'<p>Having summarized project activities and principal findings in one earlier section, and having listed publications and other specific products in another, here say how all those fit into and contribute to the base of knowledge, theory, and research and pedagogical methods in the principal disciplinary field(s) of the project.</p><p>Please begin with a summary that an intelligent lay audience can understand (Scientific-American style). Then, if needed and appropriate, elaborate technically for those more knowledgeable in your field(s).</p><p>How you define your field or discipline matters less to NSF than that you cover (here or under the next category – "Contributions to Other Disciplines") all contributions your work has made to science and engineering knowledge and technique. Make the most reasonable distinction you can. In general, by "field" or "discipline" we have in mind what corresponds with a single academic department or a single disciplinary NSF division rather than a subfield corresponding with an NSF program – physics rather than nuclear physics, mechanical engineering rather than tribology, and so forth. If you know the coverage of a corresponding NSF disciplinary division, we would welcome your using that coverage as a guide.</p>',
		'why'=>'<p>A primary function of NSF support for research and education – along with training of people – is to help build a base of knowledge, theory, and technique in the relevant fields. That base will be drawn on many times and far into the future, often in ways that cannot be specifically predicted, to meet the needs of the nation and of people. Most NSF-supported research and education projects should be producing contributions to the base of knowledge and technique in the immediately relevant field(s).</p>'
	),
	'field_er_nsf_wout_discipline'=>array(
		'title'=>'Contributions to Other Disciplines',
		'example'=>'',
		'what'=>'<p>Please identify any currently evident ways in which the project has contributed, or seems likely to contribute, to disciplines of science and engineering other than disciplines covered under "Contribution within Discipline".</p>',
		'why'=>'<p>Many fields of science, and therefore many NSF programs and projects, contribute tools or underpinnings to other fields of science. (For example, a theoretical advance in physics may have applications in chemistry or mechanical engineering.) NSF does not routinely expect identifiable applications for other fields from individual projects. Still, such applications often do arise (sometimes in ways completely unexpected when the project was initiated). They are often important results from NSF-funded projects. We want to know about them and report them, and to give credit for them where it is due.</p>'
	),
	'field_er_nsf_human_resources'=>array(
		'title'=>'Contributions to Human Resources Development',
		'example'=>'',
		'what'=>'<p>Describe how your project has contributed to human resource development in science, engineering, and technology by:</p><ul> <li>providing opportunities for research,teaching and mentoring in science and engineering areas;</li> <li>improving the performance, skills, or attitudes of members of underrepresented groups that will improve their access to or retention in research and teaching careers;</li> <li>developing and disseminating new educational materials or providing scholarships; or</li> <li>providing exposure to science and technology for pre-college teachers, young people, and other non-scientist members of the public.</li></ul>',
		'why'=>'<p>A major aim of NSF programs is to contribute to the human-resource base for science and technology, including the base of understanding among those who are not themselves scientists or engineers. A core NSF strategy is to encourage integration of research and education. NSF needs to know and be able to describe how the work we support actually furthers that aim and that strategy. Moreover, contributions of this sort are important in the evaluation of results from your project when we and reviewers are considering a new proposal.</p>'
	),
	'field_er_nsf_research_education'=>array(
		'title'=>'Contributions to Resources for Research and Education',
		'example'=>'',
		'what'=>'<p>To the extent you have not already done so in describing project activities and products, please identify ways, if any, in which the project has contributed to resources for research and education used beyond your own group and immediate colleagues, by creating or upgrading:</p><ul> <li>physical resources such as facilities, laboratories, instruments, or the like;</li> <li>institutional resources for research and education (such as establishment or sustenance of societies or organizations); or</li> <li>information resources, electronic means for accessing such resources or for scientific communication, or the like.</li></ul>',
		'why'=>'<p>Physical, institutional, and information resources are important parts of the science and technology base that NSF seeks to sustain and build. Where particular projects build or sustain those resources for a broader community of scientists, engineers, technologists, and educators, that is a significant outcome which should be counted among the results that have come from federal support of science and engineering research and education. And you should get credit for those results.</p><p>Some NSF projects serve this purpose in a direct and primary way and so might report the outputs in earlier sections. Many NSF projects do not serve it at all, and are not expected to. But many serve it in ways ancillary to their primary purposes and activities. This is the place to report such contributions.</p>'
	),
	'field_er_nsf_beyond_science'=>array(
		'title'=>'Contributions Beyond Science and Engineering',
		'example'=>'',
		'what'=>'<p>Please identify any currently evident ways in which the project has contributed to society, or seems likely to contribute, beyond the bounds of science and engineering as such. For example, the project may have contributed to the environment, commercial technology, public health or safety, economic or other policy, solution of social problems, or other aspects of the public welfare.</p>',
		'why'=>'<p>NSF expects that its broad programs will contribute to commercial technology, cost-effective environmental protection, solution of social problems, and other aspects of the public welfare by building our nation\'s science and technology base, which will then be drawn upon for all those purposes. NSF does not normally expect direct contributions of that kind from individual projects. Nonetheless, not infrequently, individual projects, or a broader set of scientific or engineering results to which the individual project has identifiably contributed, do turn out to produce more or less direct applications to the broader public welfare. That is particularly common in fields that relate more immediately to technology and other economic or social applications – engineering and computer science being only the most obvious examples. When such contributions occur, NSF should certainly report to the public and its representatives these benefits that they realize from public support of science and engineering. And again, you should get credit for such results.</p>'
	)
	);

	return $defaults;
}

/**
 * Implementation of hook_menu()
 *
 * We will create a tab on the create content page for er_nsf_fastlane, where we
 * can display the instructions which go along with the form.
 *
 * Note that we are using the access callback "node_access" which allows us to
 * make sure that this item is only visible to people who have permission to
 * create nodes of this type.
 *
 * Also note we are adding a menu item /new which will act as a proxy for the
 * node form menu item, allowing it to show up in the tabs even though it
 * was not intended to be a tabbed item.
 *
 * We are also going to add an admin section under site configuration for the
 * admin to edit the values
 *
 * @todo we really need to change the access check for the admin form here
 */
function er_overrides_menu()
{
	$items['node/add/er-nsf-fastlane/add'] = array(
		'title' => 'Create NSF Fastlane Content',
		'type' => MENU_DEFAULT_LOCAL_TASK,
		'weight' => 0,
	);
	$items['node/add/er-nsf-fastlane/instructions'] = array(
		'title' => 'Instructions',
		'description' => 'Instructions for NSF Fastlane form',
		'page callback' => 'er_er_nsf_fastlane_instructions',
		'access callback' => 'node_access',
		'access arguments' => array('create', 'er_nsf_fastlane'),
		'type' => MENU_LOCAL_TASK,
	);
	$items['admin/settings/nsf_fastlane_help'] = array(
		'title' => 'NSF Fastlane Instructions',
		'description' => 'Instructions for NSF Fastlane form',
		'page callback' => 'drupal_get_form',
		'page arguments' => array('er_er_nsf_fastlane_instructions_form'),
		'access callback' => 'node_access',
		'access arguments' => array('create', 'er_nsf_fastlane'),
		'type' => MENU_NORMAL_ITEM,
	);
	return $items;
}

/**
 * Callback to return the admin form for editing the fastlane instructions.
 */
function er_er_nsf_fastlane_instructions_form()
{
	// Set up our form array
	$form = array();

	// Get all the defaults
	$defaults = er_get_nsf_fastlane_meta_defaults();

	// Loop through them and create a fieldset for each
	foreach($defaults as $field=>$items)
	{
		// Create fieldset for each field's properties.
		$form[$field.'_fieldset'] = array(
			'#type' => 'fieldset',
			'#title' => ucwords(str_replace(array('field_er_nsf_','_'),array('',' '),$field)),
		);

		// Now loop through items and create text areas for them
		foreach($items as $item=>$value)
		{
			// Skip title
			if($item == 'title') continue;

			// Create form elements
			$form[$field.'_fieldset'][$field.'_'.$item] = array(
				'#type' => 'textarea',
				'#title' => ucwords($item),
				'#default_value' => variable_get($field.'_'.$item,$value),
			);
		}
	}

	// This handles all the saving of the items as variables, etc, for us.
	return system_settings_form($form);
}

/**
 * Callback to return the page content for our instructions tab from the info
 * in er_get_nsf_fastlane_meta()
 *
 * @return <html>
 */
function er_er_nsf_fastlane_instructions()
{
	drupal_set_title("NSF Fastlane Instructions");
	$output = '';
	foreach(er_get_nsf_fastlane_meta_defaults() as $field=>$items)
	{
		$output.='<h2>'.$items['title'].'</h2>';
		foreach($items as $item=>$value)
		{
			// Skip title
			if($item == 'title') continue;

			// Create form elements
			if($value = variable_get($field.'_'.$item,$value))
			{
				$output.='<p><b>'.ucwords($item).'</b></p>'.$value;
			}

		}
	}
	return $output;
}

/******************************************************************************
 * Example 5: We are going to tweak the workflow of the node reference field
 * which is attached to the er_external_engagement content type, and references
 * events.
 *
 * By defaut, content of the "event" type will have a link to create an
 * external engagement referencing the event. However, when clicking that link,
 * the external engagement form will only display the title of the linked event.
 * We want to show all the properties of the event, so that the user that that
 * context to use when entering the External Engagement information.
 *
 * @todo: We should change event to er_event and update its fields as well,
 * since this usage officially makes events part of our reporting application.
 *
 * @todo: We should probably have some way of marking events which have linked
 * external engagements. Or at least remove the link of an external engagement
 * has already been added for a given event.
 */

/**
 * Implementation of hook_form_FORMID_alter()
 *
 * As with the first example, we are only going to register a function here,
 * which will be called when the form is completely built. That will allow
 * us to work with the value of the CCK fields, which will not be available to
 * us before that point.
 *
 * @param <type> $form
 * @param <type> $form_state
 */


/**
 * This is our alter function. Here we have access to the completed form array.
 * We can use that to grab the node reference value, and from there load the
 * linked event node, which will allow us to display its fields so the user has
 * the context of the event to which they are attaching this external engagement.
 *
 * @param <type> $form
 * @param <type> $form_state
 * @return <type>
 */
function er_alter_event_properties($form, &$form_state)
{
	//d("Form is below2:");
	//d($form);
	$form["attachments"]["#description"] = 'Changes made to the attachments are not permanent until you save this post.';
	$form["attachments"]["wrapper"]["new"]["upload"]["#title"] = 'Attach Attendee XLS File';
	$form["attachments"]["wrapper"]["new"]["upload"]["#description"] = 'To download the Attendee Information template <a href="http://www.epscor.hawaii.edu/sites/web41.its.hawaii.edu.www.epscor.hawaii.edu/files/External-Engagement-template.xls">visit this link</a>.<br/><i>Reminder: fill out </i><b>Title</b><i> and </i><b>Reporting Year </b><i>below, and you can ignor the rest of the fields, clicking Save at the bottom of this page.</i>';
	$form["attachments"]["#title"] = '<img src="sites/web41.its.hawaii.edu.dev.epscor.hawaii.edu/themes/epscor/images/file-xls.png"/>Upload Attendee Excel File';
	$form["attachments"]["#collapsed"] = FALSE;
	// Use drupal for firebug here to see the final structure of the form.
	//firep($form);

	// If the array key we are interested in is set, and the value is not empty,
	// we know we have a calendar event to display.
	if(isset($form['field_er_cal_event_reference']))
	{
		// Create a reference for ease of reading the code
		$el =& $form['field_er_cal_event_reference'];

		//Make sure we have an actual value
		if(!empty($el[0]['nid']['#default_value']))
		{
			// Get the ID of the event we are referencing
			$nid = $el[0]['nid']['#default_value'];

			// Load that node and display it in the place of the default display
			// which would otherwise just be the node title.
			$el[0]['nid']['#display_title'] = node_view(node_load($nid),false,false,false);
		}
	}

	// Now return our modified form array.
	return $form;
}

/**
 * Implementation of hook_field_formatter_info()
 *
 * Now we want to alter the view for people viewing the External Engagement. We
 * want to display the entire calendar node within the external engagement node.
 *
 * To do this, we will define a new CCK field formatter, and take over the
 * formatting of this field. This simple provides metadata about our formatter
 * for CCK to use.
 *
 * @param <type> $node
 * @param <type> $teaser
 * @param <type> $page
 */
function er_field_formatter_info() {
  $formatters['node_reference_full_node'] = array(
	'label' => t('Node Reference Full Node'),
	'field types' => array('nodereference'),
	'description' => t('Displays the referenced node within the parent.'),
  );
  $formatters['format_custom_date'] = array(
	'label' => t('EPSCoR Custom'),
	'field types' => array('date', 'datestamp', 'datetime'),
	'multiple values' => CONTENT_HANDLE_CORE
  );
  return $formatters;
}

/*
 * DEPRECATED
 * 
 * ADDED BY ANDREW
 * This will alter the PI_Cloud block's output
 * This was done in order to rename the headings and change the order that they appear
 */
/*
function er_views_pre_render(&$view){
  //global $alter_num_results;
  if ($view->name == 'PI_Cloud'){ // && $view->current_display == 'block_1')) {
	//d($view);
	//this will reorder everything
	function sorter($a, $b){
		//i think this way would be faster than useing array_search
		$order = array(
			'RII3 leadership team'=>0,
			'Faculty participant (or equivalent)'=>1,
			'Post doc'=>2,
			'Graduate student'=>3,
			'Undergraduate student'=>4,
			'Technical support staff'=>5,
			'Non-technical support staff'=>6,
			'Other'=>7
		);
		$a_o = $order[$a->node_users_node_data_field_er_involvement_field_er_involvement_value];
		$b_o = $order[$b->node_users_node_data_field_er_involvement_field_er_involvement_value];
		$a_lname = $a->node_users_node_data_field_er_lname_field_er_lname_value;
		$b_lname = $b->node_users_node_data_field_er_lname_field_er_lname_value;
		$o = $a_o - $b_o;
		if (!$o) return strcmp($a_lname,$b_lname);
		return $o;
	}
	usort ($view->result, sorter);
	
	//this will relabel certain headers
	$labels = array(
		'RII3 leadership team'=>"Leadership team",
		'Faculty participant (or equivalent)'=>"Faculty participants",
	  	'Post doc'=>"Post docs",
		'Graduate student'=>"Graduate students",
		'Undergraduate student'=>"Undergraduate students"
	);
	foreach ($view->result as $kr=>$r){
		$l = $labels[$r->node_users_node_data_field_er_involvement_field_er_involvement_value];
		if ($l)	$r->node_users_node_data_field_er_involvement_field_er_involvement_value = $l;
	}
  }
}
*/

/*
 * ADDED BY MIKE
 * This will set up the values in the day before and day after fields for a calendar event
 * Will allow scheduling using Rules to send messages
 */
 


/*
function er_get_class_var(&$node, $loc){
	return $node->$loc;
}
*/

/*
 * ADDED BY ANDREW
 * This is part of the code that parses the external engagements uploads 
 */
function er_parse_ee_file(&$node, $file){
	require_once ER_MODULE_DIR.'/Classes/PHPExcel.php';
	require_once ER_MODULE_DIR.'/Classes/PHPExcel/IOFactory.php';
	if (!$file['new']) //don't parse the file if it's old
		return;
	//note: may or may not need file_data_path()...
	///web/1/dev.epscor.hawaii.edu/sites/web41.its.hawaii.edu.dev.epscor.hawaii.edu/files/sites/web41.its.hawaii.edu.dev.epscor.hawaii.edu/files/Web-Meeting_3.xls
	$path = $_SERVER["DOCUMENT_ROOT"].base_path()."/".$file['filepath'];
	//d($path);
	//d(file_exists($path)?"file exists":"file doesn't exist!");
	$objReader = PHPExcel_IOFactory::createReader('Excel5');
	$objPHPExcel = $objReader->load($path);
	$worksheet = $objPHPExcel->getActiveSheet();
	//$worksheet = $objPHPExcel->setActiveSheetIndex(0); //initialize to the first page...

	/* Excel Sheet Key:
	 * (2,4) title
	 * (2,5) location
	 * (2,6) date
	 * (2,7) poc
	 * (2,8) notes
	 * (0,14+) #
	 * (1,14+) name
	 * (2,14+) inst code
	 * (3,14+) person code
	 * (4,14+) gender
	 * (5,14+) minority
	 * (6,14+) paid?
	 */
	
	//really simple validation; this just makes sure that it's the expected excel sheet by checking for the header text.
	if ($worksheet->getCellByColumnAndRow(2, 1)->getValue() != "External Engagement Reporting Sheet")
		break;
	
	$title = $worksheet->getCellByColumnAndRow(2, 4)->getValue();
	//d($title);
	$location = $worksheet->getCellByColumnAndRow(2, 5)->getValue();
	//d($location);
	$date = gmdate('m-d-Y',PHPExcel_Shared_Date::ExcelToPHP($worksheet->getCellByColumnAndRow(2, 6)->getValue()));
	//d($date);
	$poc = $worksheet->getCellByColumnAndRow(2, 7)->getValue();
	//d($poc);
	$notes = $worksheet->getCellByColumnAndRow(2, 8)->getValue();
	//d($notes);
	
	/* 1 = Academic Research * 2 = Primary Undergrad. * 3 = Minority Serving * 4 = K-12 * //5 = Other (pls specify) */
	$instCodes = array('1'=>'ari', '2'=>'pui', '3'=>'msi', '4'=>'k12i');
	/* 1 = Faculty * 2 = Student * 3 = Teacher * //4 = Other (pls specify) */
	$personCodes1 = array('1'=>'tec', '2'=>'stud', '3'=>'tec'); //used for k12i
	$personCodes2 = array('1'=>'fac', '2'=>'stu', '3'=>'fac'); //used for everything else
	$genderCodes = array('m'=>'male', 'f'=>'female');
	$genderCodes = array('m'=>'male', 'f'=>'female', ''=>'und');
	$genderCodes = array('m'=>'male', 'f'=>'female');
	$minorityCodes = array('y'=>'urm');
	$highestRow = $worksheet->getHighestRow();
	
	//Use this to clear out all of the values in preparation for new values
	foreach (array("ari_", "pui_", "msi_", "k12i_", "oth_") as $inst){
		$affis = array(""); //for "other"
		if ($inst == "k12i_"){
			$affis = array("tec_", "stud_", "stut_"); //don't need stut because i wont be filling it up with anything.
		}else if ($inst != "oth_"){
			$affis = array("fac_", "stu_");
		}
		foreach($affis as $affi){
			foreach (array("total", "male", "female", "und", "urm") as $attr){
				$node->{"field_er_$inst$affi$attr"}[0]['value'] = $node->{"field_er_tot_$attr"}[0]['value'] = 0;
			}
		}
	}	

	for ($y = 14; $y<=$highestRow; $y++){
		//$number = $worksheet->getCellByColumnAndRow(0, $y)->getValue();
		$name = $worksheet->getCellByColumnAndRow(1, $y)->getValue();
		$inst = $instCodes[$worksheet->getCellByColumnAndRow(2, $y)->getValue()];
		$person = $worksheet->getCellByColumnAndRow(3, $y)->getValue();
		$person = $inst=='k12i'?$personCodes1[$person]:$personCodes2[$person]; //different for k12i
		$gender = $genderCodes[strtolower($worksheet->getCellByColumnAndRow(4, $y)->getValue())];
		$minority = $minorityCodes[$worksheet->getCellByColumnAndRow(5, $y)->getValue()];
		$paid = $worksheet->getCellByColumnAndRow(6, $y)->getValue(); //has no bearing on this form
		
		if (!$name && !$inst && !$person) //if the information dries up, just assume this is the end of the list.
			break;
		
		$col = $inst.'_'.$person;
		if (!$person || !$inst)
			$col = "oth";
		
		if ($gender)
			$node->{"field_er_{$col}_{$gender}"}[0]['value']++;
		else {
			$node->{"field_er_{$col}_und"}[0]['value']++;	
		}
		
		if ($minority)
			$node->{"field_er_{$col}_{$minority}"}[0]['value']++;

		$node->{"field_er_{$col}_total"}[0]['value']++;
	}	

	//fill up the totals
	foreach (array("ari_", "pui_", "msi_", "k12i_", "oth_") as $inst){
		$affis = array(""); //for "other"
		if ($inst == "k12i_"){
			$affis = array("tec_", "stud_");//, "stut"); //don't need stut because i wont be filling it up with anything.
		}else if ($inst != "oth_"){
			$affis = array("fac_", "stu_");
		}
		foreach($affis as $affi){
			foreach (array("total", "male", "female", "und", "urm") as $attr){
				$node->{"field_er_tot_$attr"}[0]['value'] += $node->{"field_er_$inst$affi$attr"}[0]['value'];
				if (empty($node->{"field_er_$inst$affi$attr"}[0]['value'])) $node->{"field_er_$inst$affi$attr"}[0]['value'] = 0;
				if ("field_er_$inst$affi$attr" == 'field_er_k12i_stut_male') d('HEREZ');
				// d("field_er_$inst$affi$attr", 'all');
			}
		}
	}
	$message = 'The attachment "%fn" was parsed and it\'s values were inserted into the node. Please verify that the counts are correct.';
	$vars = array('%fn'=>$file['filename']);
	watchdog('er', $message, $vars, WATCHDOG_INFO);
	drupal_set_message(t($message, $vars));
}

function _create_new_node($node, $user){
		//the node doesn't exist, so we create it.
		$newNode = new StdClass;
		$newNode->type = 'er_paid';
		// MIKE EDIT - the below wasn't working, let's try without ->name, that might be giving the token name
		$newNode->title = $node->title.' - '.realname_get_user($user['uid'])->name;
		//$clean = preg_replace('/&.*;/', '', $node->title); // this will take out ' and other html stuff &#039;
		//$newNode->title = $clean . $user['uid'] . 'new';
		//d(realname_get_user($user['uid']), 'the realname thing that isnt a string');
		$newNode->uid = $user['uid'];
		$newNode->created = strtotime("now");
		$newNode->changed = strtotime("now");
		$newNode->status = 1;
		$newNode->comment = 0;
		$newNode->promote = 0;
		$newNode->moderate = 0;
		$newNode->sticky = 0;
		
		// add CCK field data
		//$newNode->field_er_paid_user[0]['uid'] = $user['uid'];
		$newNode->field_er_paid_node[0]['nid'] = $node->nid;
		
		// save node
		node_save($newNode);
}

function _generate_node($type, $title="Untitled"){
		global $user;
		$newNode = new StdClass;
		$newNode->type = $type;
		$newNode->title = $title; //.' - '.realname_get_user($user->uid)->name;
		$newNode->uid = $user->uid;
		$newNode->created = strtotime("now");
		$newNode->changed = strtotime("now");
		$newNode->status = 1;
		$newNode->comment = 0;
		$newNode->promote = 0;
		$newNode->moderate = 0;
		$newNode->sticky = 0;
		
		// save node
		return $newNode;
}

function _create_new_paid_node($node, $user){//the node doesn't exist, so we create it.
		
		// MIKE EDIT - the below wasn't working, let's try without ->name, that might be giving the token name
		$title = $node->title.' - '.realname_get_user($user['uid'])->name;
		//$clean = preg_replace('/&.*;/', '', $node->title); // this will take out ' and other html stuff &#039;
		//$newNode->title = $clean . $user['uid'] . 'new';
		//d(realname_get_user($user['uid']), 'the realname thing that isnt a string');
		$newNode = _generate_node('er_paid', $title);
		$newNode->uid = $user['uid'];
		
		// add CCK field data
		//$newNode->field_er_paid_user[0]['uid'] = $user['uid'];
		$newNode->field_er_paid_node[0]['nid'] = $node->nid;
		
		// save node
		node_save($newNode);
}

function _manage_paid_nodes($node, $op){
	if($node->type == 'er_proposal' && $node->field_er_proposal_paid[0]['value']){
		//retrieve the list of nodes currently attached to this CnP
		$q = new Query1();
		$t = new Table1('content_type_er_paid', 'p'); //poorly labeled, this is pointing to proposal nodes
		$n = new Table1('{node}', 'n');
		$t  ->field('nid')
			->field('field_er_paid_node', 'nidr')
			//->field('field_er_paid_user_uid', 'uidr')
			->where('nidr', $node->nid)//->where('uid', $user['uid'])
			->select('nid');//->select('nidr');//->select('uidr');
		$n->field('nid')->field('uid')->select('uid');//->where('uid', $user['uid']);
		$returned = $q->addTable($t)->addTable($n)->link($t, $n, 'nid')->getData(); //this becomes the delete list
		$reference = Array();
		if ($op != 'delete'){ //if the node is being deleted, delete all paid nodes as well.
			$reference = $node->field_er_user_reference; //this becomes the new node list
			//d($returned, 'returned:');
			//d($reference, 'referenced:');
			foreach ($reference as $uk=>$user){
				foreach ($returned as $dk=>$data){
					if ($user['uid'] == $data['uid']){
						unset($reference[$uk]);
						unset($returned[$dk]);
					}
				}
			}
		}

		//d($returned, 'returned:');
		//d($reference, 'referenced:');
		$refs = count($reference);
		foreach ($reference as $uk=>$user){//add these new nodes
			//d('adding user:'.$user['uid']);
			$ret = true;
			_create_new_node($node, $user);
		}
		$rets = count($returned);
		foreach ($returned as $dk=>$data){//delete these nodes
			//NOTE: This function requires that the user has sufficient priveles to delete the node in question
			//d('Deleting: '.$data['nid']);
			node_delete($data['nid']); 
		}
		if ($refs) _node_set_published($node->nid, false); //new nodes were created
		if ($rets) _check_paid_status($node->nid); //If something was deleted, it's possible that the cnp is now valid.
	}
}

function _check_paid_status($nid){
	$q = new Query1();
	$t = new Table1('content_type_er_paid', 'p');
	$t  ->field('field_er_paid_node', 'nidr')->where('nidr', $nid)
		->field('field_er_funded_effort', 'effort')->where('effort', NULL)
		->field('field_er_funding', 'funding')->where('funding', NULL);
		//this is a special case, the first of it's kind. 
		//Turns out it's not necessary because the fields are required (but it is working).
		/*->where(array(
			array('funding', NULL, array('op'=>'IS')),//OR
			array('effort', NULL, array('op'=>'IS'))
		))*/
		//->select('effort')->select('funding');
	$returned = $q->addTable($t)->getObject();
	_node_set_published($nid, !$returned->count);
}

function _node_set_published($nid, $pub = false){
	$pub = intval($pub);
	$query = "UPDATE {node} SET status=$pub WHERE nid=$nid";
	db_query($query);
}

function _manage_ee_nodes($node, $op){
	//static $types = array('ari_fac','ari_stu','pui_fac','pui_stu','msi_fac','msi_stu','k12i_tec','k12i_stud','k12i_stut','oth');
	//static $catLabels = array("Academic Research Institutions", "Primary Undergraduate Institutions", "Minority Serving Institutions", "K-12 Institutions", "Other");
	static $types = array(
		'ari_fac'=>"Academic Research Institutions",
		'ari_stu'=>"Academic Research Institutions",
		'pui_fac'=>"Primary Undergraduate Institutions",
		'pui_stu'=>"Primary Undergraduate Institutions",
		'msi_fac'=>"Minority Serving Institutions",
		'msi_stu'=>"Minority Serving Institutions",
		'k12i_tec'=>"K-12 Institutions",
		'k12i_stud'=>"K-12 Institutions",
		'k12i_stut'=>"K-12 Institutions",
		'oth'=>"Other"
	);
	if ($node->type == "er_engagement" && $op == 'presave'){
		//d($node, $op);
		$node->field_er_tot_total[0]['value'] = 0;
		$node->field_er_tot_male[0]['value'] = 0;
		$node->field_er_tot_female[0]['value'] = 0;
		$node->field_er_tot_und[0]['value'] = 0;
		$node->field_er_tot_urm[0]['value'] = 0;
		foreach ($types as $type=>$categoryLabel){
			$male = $node->{'field_er_'.$type.'_male'}[0]['value'];
			$female = $node->{'field_er_'.$type.'_female'}[0]['value'];
			$und= $node->{'field_er_'.$type.'_und'}[0]['value'];
			$urm = $node->{'field_er_'.$type.'_urm'}[0]['value'];
			$total = $node->{'field_er_'.$type.'_total'}[0]['value'] = $male + $female + $und;
			$node->field_er_tot_total[0]['value'] += $total;
			$node->field_er_tot_male[0]['value'] += $male;
			$node->field_er_tot_female[0]['value'] += $female;
			$node->field_er_tot_und[0]['value'] += $und;
			$node->field_er_tot_urm[0]['value'] += $urm;
		}
	}else if ($node->type == "er_engagement" && $op == 'validate'){
		//d($node, $op);
		$cinfo = _content_type_info();
		foreach ($types as $type=>$categoryLabel){
			$male = $node->{'field_er_'.$type.'_male'}[0]['value'];
			$female = $node->{'field_er_'.$type.'_female'}[0]['value'];
			$und = $node->{'field_er_'.$type.'_und'}[0]['value'];
			$urm = $node->{'field_er_'.$type.'_urm'}[0]['value'];
			$total = $male + $female + $und;
			$urmLabel = $cinfo['fields']['field_er_'.$type.'_urm']['widget']['label'];
			if ($urm > $total)
				form_set_error('field_er_'.$type.'_urm][i][value', "The values for <u>$urmLabel</u> must be less than the total. See: <u>$categoryLabel</u> section.");
		}
		
		if ($node->nid == NULL){//NEW NODE IS BEING SAVED
			//$node->field_er_collab_names;
			$t = new Table('content_type_er_engagement', 't');
			$t	->field('field_er_cal_event_reference', 'event')->where('event', $node->field_er_cal_event_reference[0]['nid'])
				->field('nid')->select('nid')->select('event');
			$q = new Query($t);
			$data = $q->getData();
			if (count($data) > 0){
				$node->validated = false;
				$path = url('node/'.$data[0]['nid'].'/edit').'?destination=node/'.$data[0]['event'];
				$link = l('node', 'node/'.$data[0]['nid'].'/edit', array('query'=>array('destination'=>'node/'.$data[0]['event'])));
				//header("location: ".url('node/'.$data[0]['nid'].'/edit').'?destination=node/'.$data[0]['proj']);
				form_set_error('text', "Sorry, node could not be created! A $link already exists for this event!");
				//drupal_set_message("Sorry, node could not be created! A node already exists for this institution!", 'error');
			}
		}
	}
}

function _manage_collaborator_nodes($node, $op){
	if ($node->type == "er_collaborator" && $node->nid == NULL){
		//$node->field_er_collab_names;
		$t = new Table('content_type_er_collaborator', 't');
		$t	->field('field_er_collab_project', 'proj')->where('proj', $node->field_er_collab_project[0]['nid'])
			->field('field_er_collab_inst', 'inst')->where('inst', $node->field_er_collab_inst[0]['nid'])
			->field('nid')->select('nid')->select('proj');
		$q = new Query($t);
		$data = $q->getData();
		if (count($data) > 0){
			$node->validated = false;
			$path = url('node/'.$data[0]['nid'].'/edit').'?destination=node/'.$data[0]['proj'];
			$link = l('node', 'node/'.$data[0]['nid'].'/edit', array('query'=>array('destination'=>'node/'.$data[0]['proj'])));
			//header("location: ".url('node/'.$data[0]['nid'].'/edit').'?destination=node/'.$data[0]['proj']);
			form_set_error('Oops!', "Sorry, node could not be created! A $link already exists for this institution!");
			//drupal_set_message("Sorry, node could not be created! A node already exists for this institution!", 'error');
		}
	}
}

function er_nodeapi(&$node, $op, $a3 = NULL, $a4  = NULL){
	// d($node, 'node at top of nodeapi');
	//d($op, 'op at top of nodeapi');
   switch ($op) {
	  case 'validate':
		_manage_collaborator_nodes($node, $op);
	    _manage_ee_nodes($node, $op);
		break;
	  case 'delete':
		_manage_paid_nodes($node, $op);
		break;
	  case 'presave':
		_manage_ee_nodes($node, $op);
		if ($node->type == 'er_project'){// && $node->field_er_marker[0]['value'] == ""
				$markers = array();
				foreach ($node->field_er_research_teams as $team)
					$markers[] = strtolower($team['value']);
				$node->field_er_marker[0]['value'] = implode("_", $markers);
		}else if ($node->type == 'er_engagement' && $node->files){
			foreach($node->files as $file){
				er_parse_ee_file($node, $file);
			}
		}
		//d($node, 'presaving');
		break;
	  case 'prepare': //preparing the form..?
		// d('prepare in nodeapi');  
		//need to reset the marker or people without privs to see the field will get an error when editing a project.
		if ($node->type == 'er_project'){
			$marker = $node->field_er_marker[0]['value'];
			if (strpos($marker,'cyber') !== false
				|| strpos($marker,'dew') !== false
				|| strpos($marker,'ecogem') !== false
				|| strpos($marker,'ender') !== false){
				$node->field_er_marker[0]['value'] = "";
			}
		}
		
		// this is an array of all Content Types that contain field_er_user_reference
		$haveUR = array('er_event','ddblock_news_item','er_publication','er_presentation','er_poster','er_book','er_proposal', 'er_collaboration','er_other_product', 'er_nsf_fastlane','er_patent','er_project','er_internet');
		if (!isset($node->nid) && in_array($node->type, $haveUR)) {
			// There is not NID set, they are creating a NEW node
			$node->field_er_user_reference[0]['uid'] = $node->uid;
		}
		break;
	  case 'insert'://if the node is inserted in the database
		_manage_paid_nodes($node, $op);
		//_manage_collab_nodes($node);
		if($node->type == 'er_event') {//mike's code to set the fields for day before and day after
			// Day before
			$daybefore = strtotime('-1 day +10 hours', strtotime($node->field_er_event_date[0]['value']));
			$daybefore = date('Y-m-j\TH:i:s', $daybefore);
			$node->field_er_event_day_before[0]['value'] = $daybefore;
			
			// Day after
			$dayafter = strtotime('+1 day +10 hours', strtotime($node->field_er_event_date[0]['value2']));
			$dayafter = date('Y-m-j\TH:i:s', $dayafter);
			$node->field_er_event_day_after[0]['value'] = $dayafter;
		   
		   //d($node, 'this is node in insert');
		}
		break;
	  case 'update':
		_manage_paid_nodes($node, $op);
		//_manage_collab_nodes($node);
		if($node->type == 'er_paid'){
		   _check_paid_status($node->field_er_paid_node[0]['nid']);
		}
		break;
	  case 'view':
		// d('view in nodeapi');
		// d($node, 'nodeapi view $node');
		$updateStr = showUpdatedBy($node->nid); 
		if ($updateStr == ''){
			// not updated
			 $foot = '<br/><span class="submitted"><em>'. t('Submitted by !username on !datetime', array('!username' => $node->name, '!datetime' => format_date($node->changed))) . '</em></span>';
			 $node->content['last'] = array(
				'#value' => $foot,
				'#weight' => 99,
			 );		  				
		}      	
		else{
			// updated
			// print $updateStr;
			$node->content['last'] = array(
				'#value' => '<br/>'. $updateStr,
				'#weight' => 99,
			);								
		}	
		// Use some jQuery to change the "unpublished" text to read "paid items pending"
		//if ($node->nid == 2077) mike_nodeapi($node, $op);
		if ($node->type == 'er_proposal'){
			//d($node, 'finalview');
			$bothlines = '<script type="text/javascript">$(document).ready(function(){
							var $secondline = $(\'<div class="unpublished">pending</div>\');
							$(\'div.unpublished\').html(\'paid items\').append($secondline);
						});
						</script>';
			$node->content['first'] = array(
				'#value' => $bothlines,
				'#weight' => 0,
			);
			// here's where we hide the attached and field view for the paid nodes
			if (!isUserRefCreatedAdmin($node)){
				// if the user is NOT supposed to see the sensitive CnP field and view
				if (isset($node->content['field_er_award_amount'])) unset($node->content['field_er_award_amount']);
				$node->content['#pre_render'][] = 'removeCnPview';
			}				
		}
		break;
   }
   //if ($node->type == 'er_project')
   //	drupal_set_message('Debugging('.$op.'):'.$node->field_er_marker[0]['value'], 'warning');
}

function removeCnPview($node){
	// d($node, 'node in havetoLive()');
	
 	// $newnode = new stdClass;
	// if (isset($node['field_er_user_reference'])) $newnode->field_er_user_reference = $node['field_er_user_reference'];
	// if (isset($form['uid']['#value'])) $newnode->uid = $form['uid']['#value'];	
	
	
	
	// print 'perm.'.isUserRefCreatedAdmin($node).'.';
 	if (isset($node['show_paid_in_proposal_node_content_1'])) unset($node['show_paid_in_proposal_node_content_1']);
	return $node;
}
/**
* Implementation of hook_menu_alter().
* Remember to clear the menu cache after adding/editing this function.
*/
function er_menu_alter(&$items) {
	//d($items, 'items in hook menu alter');
}

// Removes the "Add Attendee Information" link from pages that already have an External Engagement
function er_link_alter(&$links, $node, $comment = NULL) {
  if ($node->type == "er_event"){
  	if ($node->field_er_event_reminders[0]['value'] == 0){
  		//d($links, 'links uptop');
		if ($links['node_gallery_image_field_er_cal_event_reference']) unset($links['node_gallery_image_field_er_cal_event_reference']);
  	}
		$EEexists = false;
		foreach ($node->content as $k => $v){
			if (strstr($k, "show_ee_in_calendar_event")){
				$EEexists = true;
			}
		}
		if ($EEexists){
			//d("there is an external engagement here");
			//d($links, 'links eeexists');
			
			//if ($links['er_paid_field_er_cal_event_reference']) unset($links['er_paid_field_er_cal_event_reference']);
			if ($links['node_gallery_image_field_er_cal_event_reference']) unset($links['node_gallery_image_field_er_cal_event_reference']);
			}
		  else {
			//d("there is NO external engagement here");
			}
	}
  else if ($node->type == "er_collaboration"){
  	  //d($links, 'links');
	  global $user;
	  //d($user,'user');
	  if(!in_array('EPSCoR Admin', $user->roles))
	  	if($links['node_gallery_image_field_er_collab']) unset($links['node_gallery_image_field_er_collab']);
  }
}

// make it so anonymous users don't see the exposed filter to show private or Admin Only calendar events
function er_form_views_exposed_form_alter(&$form, &$form_state) {
	global $user;
	if ($user->uid == 0){		
		$form['#info']['filter-term_node_tid_depth']['label'] = '';
		$form['term_node_tid_depth']['#access'] = FALSE;
	}
}

// Mike is testing to see whether we can grant permissions on a CnP that has user references
function er_node_grants($account, $op) {
   $grants['myrealm'] = array($account->uid);
   return $grants;
}

function er_node_access_records($node) {
	if ($node->type == 'er_collaboration') {
		//For each user referenced in the CCK item, add a grant entry.
		foreach($node->field_er_user_reference as $u) {
			// make sure referenced user is not the same as the author
			// otherwise a duplicate key error pops up
			if (!empty($u['uid']) && $u['uid'] != $node->uid) {
				$grants[] = array(
					'realm' => 'myrealm',
					'gid' => $u['uid'],
					'grant_view' => TRUE,
					'grant_update' => TRUE,
					'grant_delete' => TRUE
				);
			}
		}
	   
		// now grant permissions to author
		if (!empty($node->uid) ) {
			$grants[] = array(
				'realm' => 'myrealm',
				'gid' => $node->uid,
				'grant_view' => TRUE,
				'grant_update' => TRUE,
				'grant_delete' => TRUE
			);
		}
	}
	return $grants;
} 

//function to check if CnP nodes have all users completed their profile. called from nodeapi
/*function _cnp_paid($node){
	// TODO: can probably get rid of the below if deal
	if ($node->type == 'er_proposal'){
		$userlist = $node->field_er_user_reference;
		$query = 'SELECT * FROM `content_type_er_paid` WHERE `field_er_paid_node_nid`=\'' . $node->nid . '\'';
		$result = db_query(db_rewrite_sql($query));
		while ($data = db_fetch_object($result)) {
			// loop through, removing each person from $userlist that has completed a paid node
			$node = node_load( $data->nid );
			// looking for field_er_funded_effort_value, field_er_funding_value
			if (!empty($data->field_er_funded_effort_value)){ //could be either, both are required
				foreach ($userlist as $k => $v){
					$exists = array_search($node->uid, $v);
					if ($exists){
						// unset this person from the userlist, they've completed their paid
						unset($userlist[$k]);
						break;
					}
				}
			}
		}
		// if all the people involved have completed it, the array should be empty
		if (empty($userlist)){
			return FALSE;
		} 					
		else {
			return $userlist;
		}
	}	
}

//function to set the node to unpublished
function _cnp_unpublish($node){
	// heres where to use node_save to update the status to unpublished.
	//$node = node_load($nid);
	$node->status = 0;
	$node->changed = strtotime("now");
	//node_validate($node);
	//if ($node = node_submit($node)){
		node_save($node);
	 //   drupal_set_message(t('_cnp_unpublish: Thank you, it is now unpublished.'));	 
	//}
	//else drupal_set_message(t('_cnp_unpublish: Sorry, the node has not been unpublished properly. Contact EPSCoR web admin.'));	
}

//function to set the node to published
function _cnp_publish($node){
	// heres where to use node_save to update the status to published.
	//$node = node_load($nid);
	$node->status = 1;
	$node->changed = strtotime("now");
	//node_validate($node);
	//if ($node = node_submit($node)){
		node_save($node);
	//	drupal_set_message(t('_cnp_publish: Thank you, this completes the Current and Pending Support, it is now published.'));	 
	//}
	//else drupal_set_message(t('_cnp_publish: Sorry, the node has not been updated properly. Contact EPSCoR web admin.'));	
}*/

/*
function accomplishments_views_query_alter(&$view, &$query) {
	d($view, '$view');
	d($query, '$query');
}*/

/* implements:
 * hook_views_post_render(&$view)
 
function er_views_post_render(&$view){
	if ($view->name == 'concept_map_nodes'){
		d($view->field['field_er_research_teams_value']);
		$markers = array('drupal', 'orange');
		foreach ($view->result as $k=>$v){
			//$view->result[$k]->marker = $markers[$k];
			//$view->result[$k]->node_marker = $markers[$k];
			$view->result[$k]->gmap_taxonomy_node_marker = $markers[$k];
		}
		//$view->style_plugin->options['markerfield'] = "marker";
		d($view, "post");
	}
}*/

/* implements:
 * hook_views_pre_render(&$view)
 * update: having to scrap this because i cant get to the teams data before render occurs...

function er_views_pre_render(&$view){
	if ($view->name == 'concept_map_nodes'){
		$raw_result = $view->field['field_er_research_teams_value']->view->result;
		$teams = array();
		//foreach ($raw_result as $t){
		//	$nid = $raw_result[$t]->nid;
		//	$teams[$nid][] = $raw_result[$t];
		//}
		d($view->field['field_er_research_teams_value']);
		$markers = array('cluster', 'blue', 'white', 'green');
		foreach ($view->result as $k=>$v){
			//$view->result[$k]->marker = $markers[$k];
			//$view->result[$k]->node_marker = $markers[$k];
			$view->result[$k]->gmap_taxonomy_node_marker = $markers[$k];
		}
		//$view->style_plugin->options['markerfield'] = "marker";
		d($view, "pre");
	}
} */


/**
* Implementation of hook_node_operations().
*/
function er_user_operations() {
  return array(
    'export' => array(
      'label' => t('Export to CSV FROM ER'),
      'callback' => 'user_export_csv',
    ),
  );
}

/**
* Returns CSV file to download.
* @param array $accounts
*/
function user_export_csv($users, $view) {

  $file = "\"Name\"\n";
  if (is_array($users)){
  	foreach($users as $k => $v){
  		//$file .= $k.'=>'.$v.' ';
  		$file .= '"'.realname_get_user($v)->name . '"'."\n";
		
  	}
  }
  header('Cache-Control: must-revalidate, post-check=0, pre-check=0');
  header('Content-Type: text/csv');
  header('Content-Disposition: attachment; filename="userz.csv"');
  die(print $file);
  //die(print('buggah'));
}


	
function er_form_er_user_profile_node_form_alter(&$form, &$form_state){
	// $form['#field_info']['field_er_teams']['multiple'] = 0;
	// here we're hiding the start and term dates because those should actually be in User Events
	$form['field_er_start_date']['#access'] = FALSE;	
	$form['field_er_term_date']['#access'] = FALSE;	
	
    $form['#after_build'][] = 'user_profile_after_build';		
}
	
function user_profile_after_build($form, &$form_state) {
	
	$fields = content_types('er_user_profile');
	$fields = $fields['fields'];
		
	$fieldPlain = array();
	$adminOnly = array();
	$authArr = array();
	
	foreach ($fields as $k => $v){
		$whatKine = user_profile_fields_highlighted($v, 'edit');
		if ($whatKine == 'admin'){
			$adminOnly[] = 'div[id^=edit-'.str_replace('_', '-', $k).'] label'; // used to have ' label' after ]
		}
		else if ($whatKine == 'auth'){
			$authArr[] = 'div[id^=edit-'.str_replace('_', '-', $k).'] label';
		}
		$fieldPlain[] = str_replace('_', '-', $k);
	}	
	$bothlines = '<script type="text/javascript">$(document).ready(function(){
					var $secondline = $(\'<div class="unpublished">pending</div>\');
					$(\''.implode(', ', $adminOnly). '\').addClass(\'account-admin\').attr(\'title\', \'This field is viewable by EPSCoR Admin only\');
					$(\'div.form-radios label.account-admin\').removeClass(\'account-admin\').parent(\'div.form-item\').parent(\'div.form-radios\').parent(\'div.form-item\').addClass(\'pleaseradio\');
					$("div.pleaseradio label:not(.option)").addClass("account-admin").attr(\'title\', \'This field is viewable by EPSCoR Admin only\');

					$(\'div.form-checkboxes label.account-admin\').removeClass(\'account-admin\').parent(\'div.form-item\').parent(\'div.form-checkboxes\').parent(\'div.form-item\').addClass(\'pleaseradio\');
					$("div.pleaseradio label:not(.option)").addClass("account-admin").attr(\'title\', \'This field is viewable by EPSCoR Admin only\');

					
					$(\''.implode(', ', $authArr). '\').addClass(\'account-auth\').attr(\'title\', \'This field is viewable by Authenticated EPSCoR members\');
					$(\'div.form-radios label.account-auth\').removeClass(\'account-auth\').parent(\'div.form-item\').parent(\'div.form-radios\').parent(\'div.form-item\').addClass(\'pleaseradio\');
					$("div.pleaseradio label:not(.option)").addClass("account-auth").attr(\'title\', \'This field is viewable by Authenticated EPSCoR members\');				
					
					$(\'div.form-checkboxes label.account-auth\').removeClass(\'account-auth\').parent(\'div.form-item\').parent(\'div.form-checkboxes\').parent(\'div.form-item\').addClass(\'pleaseradio\');
					$("div.pleaseradio label:not(.option)").addClass("account-auth").attr(\'title\', \'This field is viewable by EPSCoR Admin only\');
					
					

				});
				</script>';
	
	//$node->content['first'] = array(
	$form['first'] = array(
		'#value' => $bothlines,
		'#weight' => 0,
	);
		

  return $form;
}	

/* this function is called from plugins/content_types/ ..
 * it is passed a fieldgroup argument, and it knows how to check to see which
 * fields are Admin only, and highlights those in blue
 * $op can have two values, 'view' or 'edit', the structure of the arrays are different
 * 	and edit doesn't worry about outputting values like view does
 */
function user_profile_fields_highlighted($fieldgroup, $op){
	
	$block = '';
	$profile_node = content_profile_load('er_user_profile', arg(1));
	//d($profile_node, 'profile node');
	$groups = fieldgroup_groups('er_user_profile');
	
	global $user;
	//$userAnon = new stdClass;
	$userAnon = user_load(0);
	
	$userAuthEPSCOR = new stdClass; 
	$newrolesAuthEPSCOR = array(8 => 'Authenticated EPSCoR');
	$newrolesAdmin = array(3 => 'Web Admin', 5 => 'EPSCoR Admin', 12 => 'administrator');
	
	// Add all fields that shouldn't appear 
	$dontShow = array('field_er_start_date', 'field_er_term_date');
		
	foreach ($newrolesAdmin as $key => $value) {
		if (array_key_exists($key, $user->roles)){
			$user->isadmin = true; // add a temporary flag to the user object
			break;
		}		
	}
	//d($user, 'whereroles');


	
	$userAuthEPSCOR->roles = $newrolesAuthEPSCOR;
	$userAdmin = user_load(1);	
	
	// Uncomment 2 lines below for Live Site roles
	//$newrolesAdmin = array('EPSCoR Admin', 'Web Admin');
	//$userAdmin->roles = array_merge($userAdmin->roles, $newrolesAdmin);
	switch ($op ){
		case 'view':
			if (is_array($groups[$fieldgroup]['fields'])){
					
					foreach ($groups[$fieldgroup]['fields'] as $k => $v){
						//d($v['field_name'] . ' ' . $v['label'], 'field_name & label: ');
						//d($v, 'v');
						$field_name = $v['field_name'];
						if (in_array($field_name, $dontShow)) continue;
						
						$label = $v['label'];
						$step1 = $profile_node->$field_name;
						
						$field_value = $step1[0]['value'];
						$widget_type = $v['widget_type'];
						
						
						// make sure dates are properly formatted for start date and term date
						if ($widget_type == 'date_text' && isset($field_value)){
							// date of birth is the only field where we want day
							if ($field_name == 'field_er_dob')
								$field_value = date("m/d/Y", strtotime($field_value));
							else 
								$field_value = date("m/Y", strtotime($field_value));						
						} // the below else if make sure the files are displayed properly
						else if ($widget_type == 'filefield_widget'){
							//$field_value = '<a href="/'. $step1[0]['filepath'] . '">'.$step1[0]['filename'] . '</a>';
							$field_value = l($step1[0]['filename'], $step1[0]['filepath']);
						}
		
						
						$anonAccess = user_access("view $field_name", $userAnon);
						$authAccess = user_access("view $field_name", $userAuthEPSCOR);
						$adminAccess = user_access("view $field_name", $userAdmin);
						
						if ($field_name == 'field_er_start_date') {
							d($anonAccess, "can anon view field $field_name");
							d($authAccess, "can authenticated view field $field_name");
							d($adminAccess, "can admin view field $field_name");
							d('');
								
						}
						
						//Check Anonymous vs. Admin
						// if ($anonAccess == TRUE){
						if ($anonAccess == 1){
							// d("getting in here anonAccess is $anonAccess for $field_name");
							// if ($field_name == 'field_er_start_date') print 'first';
							$show_anon_legend = true;
							$block .= '<div class="account-anon"><b>'.$label.':</b> ' . $field_value .'</div>';					
						}				
						if ($anonAccess == FALSE && $authAccess == FALSE && $adminAccess == TRUE){
							// if they have access to view it OR they're viewing their own profile
							// d("second for $field_name");
							// if ($field_name == 'field_er_start_date') print 'second';
							if ( user_access("view $field_name", $user) || $profile_node->uid == $user->uid){
								$show_admin_legend = true;
								$block .= '<div class="account-admin"><b>'.$label.':</b> ' . $field_value .'</div>';
							}
						}
						
						//Check Authenticated vs. Anonymous
						else if ($anonAccess == FALSE && $authAccess == TRUE && $adminAccess == TRUE){
							
							// d("third for $field_name");
							// if ($field_name == 'field_er_start_date') print 'third';
							if ( user_access("view $field_name", $user) || $profile_node->uid == $user->uid){ //checks to see if current user can see it
								$show_auth_legend = true;
								$block .= '<div class="account-auth"><b>'.$label.':</b> ' . $field_value .'</div>';
							}					
						}
					}
					if ($show_admin_legend) $block .= '<span class="account-admin"><span class="legend-colors">&nbsp;&nbsp;&nbsp;</span> = Admin Field</span>&nbsp;';
					if ($show_auth_legend) $block .= '<span class="account-auth"><span class="legend-colors">&nbsp;&nbsp;&nbsp;</span> = Authenticated EPSCoR</span>&nbsp;';
					if ($show_anon_legend) $block .= '<span class="account-anon"><span class="legend-colors">&nbsp;&nbsp;&nbsp;</span> = Publicly Viewable</span>&nbsp;';
			}
			break;
		case 'edit':
			$field_name = $fieldgroup['field_name'];
			$anonAccess = user_access("view $field_name", $userAnon);
			$authAccess = user_access("view $field_name", $userAuthEPSCOR);
			$adminAccess = user_access("view $field_name", $userAdmin);	
			//d($anonAccess, 'anonAccess');
			//d($field_name, 'field name');
			//d($fieldgroup, 'field group');
			
			//Check Anonymous vs. Admin
			if ($anonAccess == TRUE){
				//!d("getting in here anonAccess is $anonAccess");
				$show_anon_legend = true;
				//$block .= '<div class="account-anon"><b>'.$label.':</b> ' . $field_value .'</div>';					
			}				
			if ($anonAccess == FALSE && $authAccess == FALSE && $adminAccess == TRUE){
				if ( user_access("view $field_name", $user)){
					$show_admin_legend = true;
					//$block .= '<div class="account-admin"><b>'.$label.':</b> ' . $field_value .'</div>';
				}
			}
			
			//Check Authenticated vs. Anonymous
			else if ($anonAccess == FALSE && $authAccess == TRUE && $adminAccess == TRUE){
				if ( user_access("view $field_name", $user)){ //checks to see if current user can see it
					$show_auth_legend = true;
					//$block .= '<div class="account-auth"><b>'.$label.':</b> ' . $field_value .'</div>';
				}					
			}			
			
			if ($show_admin_legend) return 'admin';
			if ($show_auth_legend) return 'auth';
			if ($show_anon_legend) return 'anon';
					
	}

	return $block;
}

/*
function er_form_cimes_news_item_node_form_alter(&$form, &$form_state) {
	//d($form, 'form');
	//$form['field_news_summary']['#title'] = t('finally1');	
	//$form['#after_build'][] = 'ab';
	$form['#validate'][] = 'paper_validator';
	//return $form;
}
 * 
/*
function ab($form, &$form_state) {
	$what = $form['field_news_summary']['#title'];
	d($what, 'title');
	$form['field_news_summary']['#title'] = t('finally');
	//print_r($form);
	return $form;	
}
 * 
 */

function paper_validator($form, &$form_state){
	// look in either
	// form['#post']['files'] or form_state['values']['files']
	if (!isset($form_state['values']['files'])){
	    form_set_error('upload', t('You must upload a file under File Attachments.'));
	}
}


function er_admin_account() {
	dsm("SOMETHING");
  $filter = user_build_filter_query();

  $header = array(
    array(),
    array(
      'data' => t('Username'),
      'field' => 'u.name',
    ),
    array(
      'data' => t('Status'),
      'field' => 'u.status',
    ),
    t('Roles'),
    array(
      'data' => t('Member for'),
      'field' => 'u.created',
      'sort' => 'desc',
    ),
    array(
      'data' => t('Last access'),
      'field' => 'u.access',
    ),
    t('Operations'),
  );

  if ($filter['join'] != "") {
    $sql = 'SELECT DISTINCT u.uid, u.name, u.status, u.created, u.access FROM {users} u LEFT JOIN {users_roles} ur ON u.uid = ur.uid ' . $filter['join'] . ' WHERE u.uid != 0 ' . $filter['where'];
    $query_count = 'SELECT COUNT(DISTINCT u.uid) FROM {users} u LEFT JOIN {users_roles} ur ON u.uid = ur.uid ' . $filter['join'] . ' WHERE u.uid != 0 ' . $filter['where'];
  }
  else {
    $sql = 'SELECT u.uid, u.name, u.status, u.created, u.access FROM {users} u WHERE u.uid != 0 ' . $filter['where'];
    $query_count = 'SELECT COUNT(u.uid) FROM {users} u WHERE u.uid != 0 ' . $filter['where'];
  }


  $sql .= tablesort_sql($header);

  $result = pager_query($sql, 50, 0, $query_count, $filter['args']);

  $form['options'] = array(
    '#type' => 'fieldset', 
    '#title' => t('Update options'), 
    '#prefix' => '<div class="container-inline">', 
    '#suffix' => '</div>',
  );
  $options = array();
  foreach (module_invoke_all('user_operations') as $operation => $array) {
    $options[$operation] = $array['label'];
  }
  $form['options']['operation'] = array(
    '#type' => 'select', 
    '#options' => $options, 
    '#default_value' => 'unblock',
  );
  $form['options']['submit'] = array(
    '#type' => 'submit', 
    '#value' => t('Update'),
  );

  $destination = drupal_get_destination();

  $status = array(t('blocked'), t('active'));
  $roles = user_roles(TRUE);
  $accounts = array();
  while ($account = db_fetch_object($result)) {
    $accounts[$account->uid] = '';
    $form['name'][$account->uid] = array('#value' => theme('username', $account));
    $form['status'][$account->uid] =  array('#value' => $status[$account->status]);
    $users_roles = array();
    $roles_result = db_query('SELECT rid FROM {users_roles} WHERE uid = %d', $account->uid);
    while ($user_role = db_fetch_object($roles_result)) {
      $users_roles[] = $roles[$user_role->rid];
    }
    asort($users_roles);
    $form['roles'][$account->uid][0] = array('#value' => theme('item_list', $users_roles));
    $form['member_for'][$account->uid] = array('#value' => format_interval(time() - $account->created));
    $form['last_access'][$account->uid] =  array('#value' => $account->access ? t('@time ago', array('@time' => format_interval(time() - $account->access))) : t('never'));
    $form['operations'][$account->uid] = array('#value' => l(t('edit'), "user/$account->uid/edit", array('query' => $destination)));
  }
  $form['accounts'] = array(
    '#type' => 'checkboxes', 
    '#options' => $accounts,
  );
  $form['pager'] = array('#value' => theme('pager', NULL, 50, 0));

  return $form;
}

/* this function gets called by page.tpl and node.tpl to give updated dates or submitted */
function showUpdatedBy($nid, $page = false){
	if (is_null($nid)) return; 
	$format2 = 'g:i a';
	$result2 = db_query("SELECT n.title, changed, created, realname, nv.uid FROM {node} n, {node_revisions} nv, {realname} r WHERE status = 1 AND n.nid = ".$nid." AND n.vid = nv.vid AND nv.UID = r.UID ORDER BY changed DESC");
	if ($node = db_fetch_object($result2)) {
	  $output2 .= format_date($node->changed, 'custom','F j, Y - g:ia');
	}
	if (empty($node)){
		// node empty - this takes care of duplicated Submitted by calls
		return;
	}
	if ($node->changed == $node->created){
		if ($page) { // if it's a page then just print out the created time
			return "<span class=\"submitted\"><em>Submitted by ".l($node->realname, 'user/'.$node->uid).' '.$output2."</em></span>";
		} 		
		return;
	}
	else{
		return "<span class=\"submitted\"><em>Updated by ".l($node->realname, 'user/'.$node->uid).' '.$output2."</em></span>";
	}
}