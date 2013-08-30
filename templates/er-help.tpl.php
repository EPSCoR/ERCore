<?php
  drupal_add_library('system', 'ui.accordion');
?>
<h2>Introduction</h2>
<div>
  <p>This guide has been developed for first time users of the EPSCoR Core Reporting site (ER-Core).  This site has been developed using Drupal 7, an open-source content 
  management system. Whether you are a developer, tester, administrator or an end user, we all need to get familiar with Drupal first.  In addition, this guide also 
  introduces important terms used by the NSF EPSCoR program and how those terms are interpreted by the system.  This relationship is important for system administrators to 
  understand to be able to deliver reporting data to NSF with confidence.</p>

  <p>In general, most of the data requested by this reporting system will be viewable by the public, but in order to enter new, edit, or download the compiled tables you 
  will need to have an account and be logged in.</p>
</div>
<div id="er-help">
	<h3 id="request-account">
		<a href="#request-account">Request an Account</a>
	</h3>
		<div>
		<ol>
			<li>In your web browser, visit your jurisdiction's hosted Drupal site.  For UNH go to http://solstice.sr.unh.edu/ </li>
			<li>Click on the "Create new account" link located in the gray box at the top left hand side of the home page.</li>
			<li>There are 4 pages that require information, which are designated as tabs:</li>
			<ol>
				<li>Contact Information</li>
				<li>EPSCoR Involvement</li>
				<ol>
					<li><em>Most Senior Project Role:</em> use the drop down to select one role which best represents your primary role with EPSCoR</li>
					<li><em>If you are a Student Mentoring Plan Sign Off Date:</em> enter the month and year your advisor signed off on your mentoring plan.</li>
					<li><em>Please select which project component(s) you will participate in:</em> The items in this selection are determined in the customization step of 
					installing the ER-Core module. If a component you will be working on is not listed, contact your director. Otherwise, check those components you will be 
					involved with.</li>
				</ol>
				<li>Institution</li>
				<li>Account Information</li>
			</ol>
			<li>Click on Create New Account when you are done. This generates an email sent to the Web Master who needs to approve the request. </li>
			<li>An email will be sent when your account is approved</li>
		</ol>
	</div>
	<h3 id="log-in">
		<a href="#log-in">Log In</a>
	</h3>
	<div>
		<p>Once the Web Master has approved your account, an email will be sent to you.  In it will be directions to log in and include a link to visit the site.  In this 
		example we use the New Hampshire's development site.  In your web browser, visit the URL to your jurisdiction's web site.  Your program administrator needs to give this 
		to you.</p>
	</div>
	<h3 id="user">
		<a href="#user">User Profile</a>
	</h3>
	<div>
		<p>The system greatly depends on complete and accurate user data.  Please complete all the fields presented to you in the user profile.  The final data is only as good 
		as the data you enter.</p>
		<p>After you login you will be taken to your User Profile home page.  At any point in time you need to get back to your user account, scroll to the top of any page 
		and look in the black menu bar for your user name.  It actually says, "Hello (user)".  Click on your name and you will be taken to your user account.</p>
		<p> Your name is a row of tabs.  The most important of those are the View and the Edit tabs.  Clicking either will give you that functionality (click Edit to edit, and 
		View to view your profile's content).</p>
		<p>Below your photo (once you have uploaded one) is a block of horizontal tabs of grouped information.    The other tabs are Contact, Demographics, EPSCoR Involvement,  Institution, and Data Collection.  Clicking on those tabs will take you to that information.  Once in a View of our choice, clicking on Edit will allow you to edit that content.</p> 
		<p>Next note to the right are the gray ovals with the term "public" nested inside it.  We have developed an indicator system that allows you to see who can see what of 
		your user profile data.  In this case, the public can see my name and general contact information.  If I were to click on the Demographics tab, the ovals would be a 
		teal blue and include the phrase, "administrative staff", which indicates that only you (the owner) and EPSCoR administrators can see your demographic data.  The third 
		privilege indicator is "Authenticated", which means that only you, registered and logged in EPSCoR participants, and EPSCoR Administrators can see that data.</p>
		<h3 id="">
			Contact Information
		</h3>
		<ol>
			<li>Click on the "Hello (username)" link at the top right of any page  (after you are logged in)</li>
			<li>Click on the Contact Information tab if you are not already there</li>
			<li>Click on the Edit tab</li>
			<li>The red asterisk after a field means that the data is required.  You will not be able to save the form unless all required fields have been completed.</li>
			<li>First and Last name are required fields as part of your user profile. </li>
			<li>Phone fields (mobile and work), Work Address and Professional Link are optional fields.</li>
			<li>Click Save to save your contact information.</li>
		</ol>
		<h3 id="">
			Demographics
		</h3>
		<p>NSF requires that we try to obtain demographic data for all our participants, including those attending EPSCoR sponsored events.  Given such a requirement, it is 
		OPTIONAL to you if you want to submit it or not.  This data will be encrypted and disassociated from your name.  Only the counts of the various entries will be submitted 
		to NSF.</p>
		<ol>
			<li>Select the appropriate radio buttons that best describes your demographics</li>
			<ul>
				<li>In the case where you have multiple disabilities, citizenships or races, select the one that best describes you.</li>
			</ul>
			<li>Check the "I am a veteran" check box if you are a veteran</li>
		</ol>
		<h3 id="">
			 EPSCoR Involvement
		</h3>
	
		<h3 id="">
			 Most Senior Project Role (Required)
		</h3>
		<p>Select the one position type that best describes your involvement with EPSCoR.  This list is defined by NSF Fastlane. If a position type is not listed that better 
		defines your involvement, select the Other option.</p>
        
        <h3 id="">
		EPSCoR Leadership Team (Required)
		</h3>
        <p>Select the option that best fits your role.</p>
		<h3 id="">
		Project Component(s) (Required)
		</h3>
		<p>Components are analogous to teams and are unique to each jurisdiction.  Components can easily be changed by an administrator or Drupal developer.</p>
		<ol>
			<li>Select the project component of which you are involved</li>
			<li>Select multiple componenets if you are involved with more than one</li>
			<li>If a component you are involved with is not listed, contact your administrator</li>
		</ol>
		<h3 id="">
		 Please select which boards or committees you will be a member of
		</h3>
		<p>Select one or multiple (if applicable)</p>
		<h3 id="">
			 Participating Date
		</h2>
		<p>Enter the date you officially began participating with EPSCoR.  Clicking the Add another item will allow you to add multiple Participating Dates</p>
        
                	<h3 id="">
			 Hire Date
		</h3>
        <p>Enter the date that you were first hired by EPSCoR (if applicable). This would be the date that you started getting paid.</p>
        
        
		<h3 id="">
			 Are you paid by the EPSCoR grant (Required)
		</h3>
		<p>Select the option that best fits your role.</p>
        <p>If yes, the option for Months of Direct Funding will appear. From the drop down, select how many months of direct funding you receive. Round up or down to the nearest month of funding you are expected to receive.</p>
        <!--This question is no longer requiered
		<h3 id="">
			<a href="#5.7">5.7</a> How many months of direct funding do you receive
		</h2>
		<p>Select a number from the drop down box and round up or down to the nearest month of funding you are expected to receive</p>-->

		<h3 id="">
			 Do you participate in EPSCoR research more than 160 hours per year
		</h3>
		<p>This ties into NSF Fastlane and refers to whether or not you put in more thatn 160 hours of effort in a year to this EPSCoR grant, whether you are paid or not</p>
		<ol>
			<li>Click the appropriate radio button with respect to the number of hours you participate in this EPSCoR grant in a year</li>
			<li>If yes, please select the nearest person month of effort in EPSCoR</li>
			<li>Select a number from the drop down box and round up or down to the nearest month of funding you are expected to receive.</li>
		</ol>
		<h3 id="">
			 Please describe role and contribution to the project
		</h3>
		<p>Include information about what you are doing, in which projects, who you are working with, location, dates and your contribution.</p>	
      
		<h3 id="">
			 RCR - Responsible Conduct of Research
		</h3>
		<p>Click the "I have completed the RCR training" check box if you have taken either in-person or on-line versions of the RCR training.</p>
		<ul>
			<li>RCR In-Person: Enter the month, day and year of when you took the in-person course</li>
			<li>RCR In-Person Upload: Upload a txt, word or pdf of your certificate</li>
			<li>RCR On-Line: Enter the date you took the RCR online training</li>
			<li>RCR On-Line Upload: attach the txt, word or pdf of your online RCR certificate</li>
		</ul>
		<h3 id="">
			 Institution
		</h3>
		<p>Click the radio button of the institution you are affiliated with.  The house icon denotes that institution is a Participating institution with respect to your 
		EPSCoR jurisdiction.  The world icon denotes that that institution is not a participating institution.  If your institution is not listed, click the "My institution is 
		not listed here (add a new institution) check box:</p>
		<ul>
			<li>Clicking the 'add new institution check box' reveals additional fields unique to institutions</li>
			<li>Enter the name of the new institution</li>
			<li>Enter the abbreviation of the institution, if there is one</li>
			<li>Enter the Institution website link</li>
			<li>Enter the country of the institution</li>
			<li>Provide a brief description</li>
            
		</ul>
	  <p>Department: Enter in the name of the department for which you are affiliated</p>
		<h3 id="">
		 Data Collection
		</h3>
		<p>Check the "I plan to be collecting data through my research" check box if you anticipate collecting data as a result of your research.</p>
        <p>Has this user been put in touch with the Data Manager?: Check the appropriate button for your situation.  This will send notices to the EPSCoR administrator as well as the data repository manager.</p>
		<p>
		  <h3 id="">
		 Account Information
		</h3>
		</p>
		<ol>
		  <li>Username: The system user name is determined by the website administrator upon account creation. This is a required field </li>
			<li>Current Password: This is your current (non-visible) password and can be reset by clicking on the "Request new password" link near your Current password field or by entering a new Password in the two fields below your E-mail address.</li>
	
            <li>E-mail address: This will need to be A valid e-mail address. All e-mails from the system will be sent to this address. The e-mail address is not made public and will only be used if you wish to receive a new password or wish to receive certain news or notifications by e-mail.  This is a required field.
			<li>Status:  This determines whether or not you or a user can login to the system.  Leave it Active.  </li>
			<li>Roles:  These roles are the technical Drupal roles created to designate who can see or access what.  Different from Involvement, which is the role or hat one 
			wears with respect to their participation within EPSCoR.  It is strongly recommended that EPSCoR staff review the Roles and Permissions page to suit your needs at your local area. Below is what is provided in the default EPSCoR Reporting Core module. The standard roles are as follows:</li>
			<ul>
            <li>Anonymous User:  This role is used for users that don't have a user account or that are not authenticated.</li>
				<li>Authenticated user:  This role is automatically granted to all logged in users (simply means that you have an account in Drupal). </li>
                 <li>Guest: Very limited role with access to view ERCore module functionality only. </li>
                 <li>Student:  Identical to Faculty out-of-the-box. An important designation technically, but left up to the jurisdiction to further restrict or grant access to specific areas.</li>
                 <li>Faculty:  A subset of EPSCoR Administrator generally restricting the ability to edit other's content</li>
                 <li>Administrator Staff: A subset of Site Administrator and EPSCoR Administrator, restricting access to the more technical aspects of Drupal and EPSCoR reporting</li>	
                 <li>Administrator: Access to any and all Drupal capabilities</li>
			</ul>
            	<li>Flags: Clicking the Receive data collection notification tells the system to send you an email when a new user has registered in the system who anticipates 
			collecting data.</li>
            
		</ol>
		<h3 id="">
			Participant Date
		</h3>
		<ol>
			<li>Click the "Show end date" check box if you are back-entering data and know the day that you or the user's end participation date</li>
			<li>Select the month, day and year of the day you started participating with EPSCoR</li>
			<li>If you checked the "Show end date" box, a "to" date group will appear.  Enter the last day of your participation with EPSCoR</li>
			<li>Participant Date 2:  use the second grouping of dates if you had at one time been participating, then ceased, and now are involved again.  Enter your new start 
			date of when you became involved the second time.</li>
			<li>Click the "Show end date" box if you know the end date of your second involvment</li>
			<li>Use the "Add another item" to add a PARTICIPANT DATE 3 group.</li>
			<li>You can also rearrange the dates by a click-and-drag of the 4-way arrow icon to the left of the date group.</li>
		</ol>
		<h3 id="">
			 Administrative Overlay
		</h3>
		<p>Disregard this option if you see it.</p>
		
		<h3 id="">
			 Upload a photo
		</h3>
		<p>Click on the Choose file button to browse and upload a photo.</p>
        <h3 id="">
			Locale Settings
		</h3>
		<p>Select your locale setting.  This will configure Drupal to show date and time to your location.</p>
		<h3 id="">
			Save
		</h3>
		<p>Click the Save button when you are done. If any required fields are incorrectly or not answered, an error message will be displayed at the top of the page.  Read it 
		and address the issue.  If for some reason you cannot address the issue, call your EPSCoR administrator or your Drupal web site manager for help.</p>
	</div>
	<h3 id="enter-data">
		<a href="#enter-data">Introduction to Entering Reporting Data</a>
	</h3>
	<div>
		<p>This guide is written with the assumption that you are working with a default ER-Core installation.  Logged in userss will see a black menu across the top of the 
		browser, just under the tabs of the browser.  The default ER Core installation will show this administrative menu and this guide is written assuming it is visible.  
		Each jurisdiction will have the option to change the layout of the site and how menus look and are placed.</p>
		<p>The 4th link from the left in the black administration menu is Content.  Hover your mouse over the link to see nested drop down menus.  Hover over the Add Content 
		link to expand yet a 2nd nested menu list that contains links to the default forms that come with standard Drupal installation, plus links to the ER Core reporting forms, 
		and custom jurisdictional forms.  </p>
		<p>Hover over the Content link to show the expanded menu and click on the Add Content link.  You will see a page that has the various types of content you can 
		add grouped into the headings of Drupal Conent, EPSCoR Reporting Categories, and Jurisdictional.  </p>
		<p>Drupal Content is the type of content that Drupal comes with during a standard installation such as article, page, blog, image, etc.  Your jurisdiction can add or 
		remove items from this group.  </p>
		<p>EPSCoR Reporting Categories are the content types that come with the ER-Core module and represent the minimal reporting requirements to populate the reporting tables.  </p>
		<p>The Jurisdictional group includes items that are optional or custom to that jurisdiction.  During installation your Drupal expert has the option to install ER-Core 
		sub-modules or not.  These sub-modules are optional and when installed will appear in this group. </p>
  </div>
  <h3 id="er-event">
    <a href="#er-event">Calendar Events</a>
  </h3>
  <div>
		<p>The Calendar Event and External Engagement content types are unique content types, i.e., they have their respective forms and views but they are linked to each other. The general process is to enter a calendar event and denote whether or not the event will be attended or not by the public. If it is, then the external engagement form will be made available after the event has passed in time. To get to the external engagement form you visit the calendar event and look for the external engagement link down near the bottom (specific steps outlined below).</p>
		<p>A Calendar Event (node) is the typical data that represents an event. The External Engagement data (node) is linked to a specific Calendar Event when the event will be attended by non-EPSCoR participants and includes the counts by demographic breakout of the public that attended the event.</p>
		<p>To create a Calendar Event:</p>
		<ol>
			<li>Find the Calendar Event link from within the Add Content drop down from the black administrative menu or on the Create Content page.
			<li>Title: Enter the title of the event
			<li>Date:  Enter the date of the event
			<ul>
				<li>Show End Date:  Click the Show End Date box if you want to designate this event with an ending period or time.
				<li>Click the Repeat box if this event recurs on a regular schedule
			</ul>
			<li>EPSCoR Organizers:  type in the first or last name of the point of contact or organizer for this event.  This person must be an EPSCoR participant (registered in 
			the system).
			<li>Event Type:  Select from the drop down list the type of event this will be.
			<li>URL:  enter the link to the event if it is represented by another web site
			<li>Will this event be attended by non-EPSCoR participants:  click the appropriate radio button.  Clicking Yes will link the External Engagement form and data to 
			this event.
			<ul>
				<li>Note:  an email will be sent to the point of contact informing them that external engagement data will need to be captured, which will include an Excel 
				template to assist with collecting that data.
				<li>External Engagement data will be discussed in detail following this Calendar Event section
			</ul>
			<li>Abstract or Short Description:  write a short description or abstract of the event.
			<li>Send notifications of this event to:  check the boxes of the groups you wish to send an email to about this event to.
			<li>Attach Flier:  click the Browse button to attach a flier or additional file for the event.
			<li>Address:  Enter the address of the event if there is one
			<li>Building (Room Number):  Enter the building and room number if there is one
			<li>Save:  Click Save when you are done.
		</ol>
  </div>
  <h3 id="er-collaboration">
    <a href="#er-collaboration">Collaborations</a>
  </h3>
  <div>
		<p>There are two steps needed to accurately report on a collaboration.  The first step is to describe the collaboration. Give the collaboration a title, attach the 
		relevant dates, and provide a description of the effort. The second step is to attach the names of the collaborators. We ask that you provide their full name, 
		institution, and role ("Participant" or "Collaborator"). If the person has a log-in into the system, you may enter their name into the user lookup field and use the 
		information as provided in their user profile. </p>
		<p>Creating a Collaboraion</p>
		<ol>
			<li>To begin, click on the Collaboration link from the Add Content page.
				<ul>
					<li>Title: Some of the activities under the collaboration definition we provided above may not have a clearly defined "title". Nevertheless, we ask that 
					you provide some kind of semi-descriptive blurb that will be used in reference to the collaborative effort, e.g. "Visualization and Scientific Computation", 
					"Survey of Statewide K-12 STEM Programs", "Wentworth Core Genetic Facility", and so on.</li>
					<li>Project Component: Project components are meant to describe a certain branch, component, team, or department within your EPSCoR jurisdiction. This list 
					is populated by the administrator running your site, and will therefore be specific to your EPSCoR jurisdiction (or "Project"). Select the components that 
					this collaborative activity falls under.</li>
					<li>Dates: Please provide the dates during which the collaboration took place. If the collaboration is still active, simply do not provide the "end" date.</li>
					<li>Description: Please provide a brief summary of the collaborative effort, the general roles of the people involved, and any other relevant contextual 
					information. Address why you are engaged with the individuals at the other institutions and how their expertise enhances or benefits your EPSCoR efforts.</li>
				</ul>
			Once you have filled out the necessary information in this form, submit the form and proceed to step two.</li>
			<li><p>Once the collaboration's descriptive information has been entered (step one), you should automatically be directed to the page that displays the information  
			that you just entered. There should be a section labeled "Collaborators" on the right hand side of the display, and a link labeled "Add a name to this collaboration". 
			Click on this link to be brought to a new form.</p>
			<p>This form allows you to attach the name of an individual to the collaboration we set up in step one. Please note that in order to attach an institution to your 
			collaboration you must first list the name of the collaborator from that institution. That is to say, a collaboration may involve individuals at institutions, but 
			may not reference a institutions directly. If you wish to attach an institution to your collaboration, please enter the name of the primary contact at that 
			institution.</p>
			<ul>
				<li><p>User Lookup: This section allows you to attach the name of a person that has a log-in into the system, and additionally it provides you with a mechanism 
				to automatically fill out the rest of the form based on the information provided in their user profile. There are a few simple steps involved when using the user 
				lookup section.</p>
				<p>The User Lookup text-field has a special feature that allows you to search for a user in the system. Begin by typing in the name of the person you are looking 
				for. You should then see a little blue circle start to spin in the far right side of the text-field, this indicates that it's searching. When the circle stops 
				spinning (which may take a few seconds), a little panel should be displayed directly below the text-field that will contain the names of people in the system that 
				match what you typed in. Click on one of the displayed names in order to lock in your selection.  A button labeled "Auto-fill fields below" should appear once you 
				have locked in your selection. Clicking this button will reload the form with the user's information automatically filled out.</p>
				<p>If the blue disc stops spinning and no names appear, then it is likely that the person does not have a user log-in into the system, and in which case you should 
				delete the name from the User Lookup field and proceed to the Manual Entry section below.</p>
				</li>
				<li>Manual Entry Section: This information needs to be filled out for every person you wish to add to the collaboration. Note: This information may have been 
				filled out for you if you used the "Auto-fill fields" feature described above in the User Lookup section.</li>
				<li>Name: Provide the full name of the individual. This should be the name of a person, and not a location, place, establishment, etc.</li>
				<li><p>User Status: The user status determines how the system should count this individual in our reports. There are two criteria that must be met in order to be 
				considered a Participant (according to NSF), those are:</p>
				<ol>
					<li>The individual must contribute more than 160 hours per year to EPSCoR</li>
					<li>The individual must belong to a Participating Institution (an institution within EPSCoR i.e. your local Universities)</li>
				</ol>
				<p>If the user does not meet both these criteria, then they should be considered a Collaborator.</p>
				</li>
				<li><p>Institution: Please select the institution that the person was working from during the collaboration. A person may belong to several institutions, and may 
				collaborate from one of many. Select the institution that is appropriate for this collaboration.</p>
				<p>If you are looking for an institution but not finding it on the list, select the "The institution is not listed here" checkbox to expand the form to include 
				several new fields. These fields ask that you provide information about the new Institution. Fill them out to the best of your ability. Please double-check the 
				list of institutions already listed before adding a new one to the list.</p></li>
				<li><p>Institution Department/Unit: We provide you with the option of including the Department that the person is collaborating from. Please note that the system 
				counts two different departments from the same University to be two distinct "institutions" (as defined by NSF). This plays a part in how we count up the 
				collaboration information in our reports to NSF.</p>
				<p>After you have completed and saved this form, you will be returned to the main page for the collaboration. You can then add another name to the collaboration 
				by clicking on the "Add another collaborator" link located in the Collaborators section on the right side of the page.</p></li>
			</ul>	
			</li>
		</ol>
  </div>
  <h3 id="er-engagement">
    <a href="#er-engagement">External Engagements</a>
  </h3>
  <div>
		<p>If you clicked the "This event will be attended by non-EPSCoR attendees" radio button, several 'things' will be put into motion.  First a notification 24 hrs before 
		and after the event will be sent to the point of contact (POC) as identified in the Event form.  This notification will inform the POC the NSF requirement to collect the 
		data, and include an Excel template to assist with collecting the data.  Additionally, once an event is identified as being an External Engagement, the Event will be 
		denoted as "This content needs review" next to the title until the external engagement data is submitted.  </p>
        <p>To add External Engagement data to an event: You can upload the External Engagement template file, or you can input the values manually into the External Engagement form. Both methods will be explained below.</p>
		<p>There are two options for adding External Engagement data collected from a calendar event:</p>

		<ol>
			<li>Click the Add Attendee Data link at the bottom of the Calendar Event View (after the event has been saved).</li>
			<li>Calendar Event:  This is an auto lookup field and should be linked to the event at hand.  There is a one-to-one relationship between Events and EE data.  If the 
			title of the event is not showing, start typing in the title and the lookup will search for similar names.  Once the name is found, click on it to enter it into the 
			field.</li>
		</ol>
		<p>There are two options to add attendee data: </p>
		<ul>
			<li>Upload a completed Excel sheet: One way, and perhaps the easiest, to attach External Engagement data to an event is to use the Upload a completed Excel Sheet method. After an event is created and designated an External Engagement event, an email will be sent to the POC along with an External Engagement Excel template. The POC can then print out this sheet to be sent around to the event participants to be filled out, or the POC can count and estimate the demographics of the event participants him or herself. After the event, the POC can complete the template working at their PC and save that file locally. Once saved, this file can be uploaded to the system and the system will read in those values - given that the form was not altered in format.</li>
			<ol>
				<li>Click the "I WANT TO UPLOAD A COMPLETED EXCEL SHEET" and the information will expand. Download the External Engagement Reporting Sheet and fill out the data at your event. Once the sheet is completed you may then upload the excel sheet below, and the system will do the rest.</li>
				<li>After you have uploaded your completed excel sheet you may skip the "I WANT TO ENTER ATTENDEE FIGURES MANUALLY" section. Click "Save" at the bottom of the form.</li>
			</ol></li>
			<li><p>Enter attendee figures manually: Alternatively, you can enter in the counts individually.  After linking this EE data to a specific event click the groups to expand 
			or contract them as necessary: Click on "I WANT TO ENTER ATTENDEE FIGURES MANUALLY" and the information will expand.</p>
			<ol>
				<li>Total:  click on Total to see the automatically summed totals for the individual values (after the EE data has been saved).  If no values are present, no data 
				has been entered.
				<ul>
					<li>Values are estimated:  click the Values are estimated box to indicate that the counts being entered are estimated by the POC.</li>
				</ul></li>
				<li>Click through the various headings to expand and enter data for those specific categories</li>
				<li>Click save when you are done.</li>
			</ol></li>
		</ul>
  </div>
  <h3 id="er-proposal">
    <a href="#er-proposal">Proposals and Grants</a>
  </h3>
  <div>
		<p>Entering your current and pending support is a multi step process, depending on whether or not any EPSCoR participants listed on the grant have salaries that are 
		supported by the grant, and if there is more than one EPSCoR member listed on the grant.  If no one listed on the grant is receiving salary support, and there is only one 
		person listed on the grant, entering a grant is a single step process.</p>
		<ol>
			<li><p>To begin, choose the Current and Pending Support link from the Add Content link of the left hand navigation list.  Scroll down and look for the "Current and 
			Pending Support" link. Once in the Create Current and Pending Support form:</p>
			<ul>
				<li>Note 1:  Do not enter data to represent the EPSCoR grant, but all other grants are required regardless of funding agency or amounts</li>
				<li>Note 2:  NSF also requires that you enter data on grants that have been declined while you are an EPSCoR participant.</li>
			</ul>
			<ol>
				<li>Title:  Enter the official title of the grant</li>
				<li>EPSCoR Participants listed on the Grant:  Enter the names of all the EPSCoR participants listed on the grant.  This is a look-up field.  Start typing in 
				the first or last name of the participant and give the little blue wheel a chance to search the database.  If the name does not appear in the drop down choices, 
				try again using the other name.  If it still does not appear, that person is NOT has not registered with the EPSCoR web site and needs to do so.  In the meanwhile, 
				you can enter that participant's name in the Non-EPSCoR Participants section directly below.</li>
				<li>Non-EPSCoR Participants listed on the Grant:  List all the other non-EPSCoR participants on the grant in this section.  Please note that this representation of 
				the grant is for internal reporting purposes.  We do not need to know who is the PI on record, hence we don't ask for that designation.</li>
				<li>Salary Support:  If ANY EPSCoR members listed on the grant receive salary support from this grant, check the appropriate radio button</li>
				<li>Source of Support:  Enter the name of the funding agency</li>
				<li>Please Categorize the nature of the Source of Support:  Select one of the 3 choices
					<ul>
						<li>Other NSF:  key word "other".  Do not list this NSF EPSCoR grant.  List all other NSF grants you may have.</li>
						<li>Other Gov:  List all other federally funded grants you may have</li>
						<li>Other:  List all other grants not federally funded (state, local, university, private, etc.)</li>
					</ul></li>
				<li>Amount Requested:  Enter the original amount requested.</li>
				<li>Status:  Select the appropriate status of the grant.
					<ul>
						<li>Pending:  If you select Pending, please enter the Date of Pending Notification</li>
					</ul></li>
				<li>Date Submitted:  Enter the date you submitted the grant</li>
				<li>Location:  Enter the primary location of where the research will be conducted.</li>
				<li>Brief Description of Proposed Work:  Enter the summary or the abstract of the Proposed work to be done for that grant.</li>
				<li>Save:  Click the Save button to save your work.  This form will not be able to be partially completed.  All required fields are necessary to be completed in 
					order form to be saved.</li>
			</ol>
			<p>If you are the only researcher listed on the grant, and you do not receive salary support from this grant, you are done and do not need to proceed to step 2.</p></li>
			<li><p>Participant Effort is the effort each researcher listed on the grant states that they will devote in time (# cal months, academic months, or summer) to do the 
			proposed research.  This value has nothing to do with whether or not you receive salary support or not.  A "Needs Review" will show in each row of the table for the 
			researcher who have not entered in their effort.</p>
			<p>For each researcher:</p>
			<ol>
				<li>Edit:  Click the edit button to enter in the Funded Effort and number of associated Months for the researchers</li>
				<li>Award Title:  Award Title will be automatically filled with the name of the grant to which you just navigated from, OR, if blank, start typing in the name or 
				title of the grant to which you need to link this funded effort to. </li>
				<li>Individual Funded Effort:  Enter in the number of months from 0 - 12 the number of months the Funded Effort is/will go towards.  For all participants listed 
				who do not receive salary support, enter 0 for Funded Effort.</li>
				<li>Funding Amount:  Enter the Funding Amount ($) for which the Funded Effort covers.  If you do not receive salary support, enter 0 for Funding Amount.</li>
				<li>Save:  Click Save when done.</li>
			</ol>
			<p>After you click Save, you will be returned to the View for that Grant.  Repeat steps 1 - 5 of Funded Effort above for each participant listed on the grant.  Until 
			Funded Effort data is entered for each participant, the grant will be in "Draft" status and be denoted by the "This content needs review" caption.</p></li>
		</ol>
  </div>
  <h3 id="other-cts">
    <a href="#other-cts">Highlights, Institutions, Internet Dissemination, Other Research Products, Patents, Presentations, Publications</a>
  </h3>
  <div>
		<p>From any page in the site, and once you have logged in, click the Add Content link from the top left hand side navigation list or from the Content link in the black menu 
		bar across the top.</p>
		<p>From there choose the data category you wish to enter data for.  We are grouping the data categories for Highlight, Institution, Internet Dissemination, Other Research 
		Product, Patent, Poster, Presentation and Publication together because these forms are straight forward and the method to enter data for any one of these can be generalized.
		</p>
		<ol>
			<li>Select a data category</li>
			<li>Title: Enter the title</li>
			<li>EPSCoR Participants:  use the auto look-up to attach EPSCoR Participants</li>
			<li>Date:  Enter the year, month and date as appropriate</li>
			<li>Description:  enter a short description of the data content at hand</li>
			<li>Other, data category-specific fields:  the fields will vary for the different data categories.  Fill them out as appropriate.  Some will be required. Other fields 
			will not be required but please remember that more information is better, and that the data will only be as good as the data you enter.</li>
			<li>Save:  click save when done.</li>
		</ol>
	</div>
	<h3 id="viewing-data">
		<a href="#viewing-data">Viewing and Downloading Reporting Data</a>
	</h3>
	<div>
		<p>After logging in a black menu bar will appear across the top of the screen.  The default ER Core installation will show this administrative menu and this guide is 
		written assuming it is visible.  Each jurisdiction will have the option to change the layout of the site and how menus look and are placed.  This guide is written with the 
		assumption that you are working with a default installation.</p>
		<p>The 1st link from the left in the black administration menu is EPSCoR.  Hover your mouse over the link to see nested drop down menus.  Hover over the Content link to 
		expand yet a 2nd nested menu list that contains links to every form that has been filled out sorted by type. </p>
		<p>Once you have selected the type of content you wish to view by clicking on it, you will be directed to the appropriate page. This page will display a list of all pages 
		containing your selected content as well as giving you filtering options at the top of the page. </p>
		<p>If you wish to download the reporting data in excel format, you will find a link on the bottom of the page that reads "XLS". Clicking on "XLS" link will automatically 
		begin the download. </p>
	</div>
	<h3 id="editing-data">
		<a href="#editing-data">Editing Reporting Data</a>
	</h3>
	<div>
		<p>Make sure you are logged into the website and have the appropriate permissions to edit data. Once logged in use the same menu used to view data to find the entry to 
		edit. </p>
		<p>Once the entry to edit has been located click on its title to view the entry.  Then click on the EDIT tab underneath the title. This will reopen the node and allow you 
		to make changes to it. Once you are done with your changes you can save them by clicking on the save button at the bottom of the page.</p>
	</div>
	<h3 id="terms">
		<a href="#terms">Common Drupal Terms</a>
	</h3>
	<div>
    <ul>
      <li>
        <strong>Auto Lookup</strong>
        <p>A field on a form that searches for data already in the database such as a User's name.  If you were to start typing "dav" of David Kimball, the blue
        wheel to the right of the lookup field starts to spin as it is searching.  When the search is complete, the search results are displayed in a list directly below the 
        lookup field.  Use your mouse or tab down to select one of the results.  Click on it to enter that result to the field.  Note, even though you can type in any string of 
        text, unless there is a match (search result hit) and you select it from the list, the form cannot be saved.  If you are searching for a name of someone who is a project 
        member, for example, and the name does not appear, you may be misspelling the name, or that person does not have a User Profile.</p>
      </li>
      <li>
        <strong>Content</strong>
        <p>Content refers to the information or data that Users input into Drupal through Forms.</p>
      </li>
      <li>
        <strong>Content Type</strong>
        <p>A content type is one of the more technical terms used to specify the different groups of content Drupal can collect.  Examples of Content Types are 
        event, page, blog, image, collaboration, presentation, publication, etc.</p>
      </li>
      <li>
        <strong>Custom Module</strong>
        <p>A custom module is a Drupal Module that is written in the PHP programming language that has not been adopted by the Drupal Association for general 
        release and made available at Drupal.org.  Typically custom modules are written for the unique needs for certain groups.  In our case, we are developing a custom module 
        named the ER-Core and its functionality will be useful only to EPSCoR administrators.</p>
      </li>
      <li>
        <strong>Drupal</strong>
        <p>Drupal is a free and open-source content management framework (CMF) written in PHP and distributed under the GNU General Public License.</p>
      </li>
      <li>
        <strong>Field</strong>
        <p>A Field is an element of a Form where data is entered.  Fields can include such items as Dates, Titles, text boxes, etc.</p>
      </li>
      <li>
        <strong>Form</strong>
        <p>A form is the web interface that allows Users to input information or data.  Each Content Type will have a form associated with it.  The term form is rather generic.</p>
      </li>
      <li>
        <strong>Module</strong>
        <p>A module is a packaged group of PHP code that allows users to do specific defined actions.  Drupal is built on the premise that web site developers and 
        managers can easily download a module and turn it on to gain quick, easy access to functions that web sites typically utilize.  There are thousands of modules to 
        choose from.  Example of typical modules are calendar, image gallery, event registration forms, etc.</p>
      </li>
      <li>
        <strong>Node</strong>
        <p>A Node refers to the group of information or data that comes into the database through a form.  If you wanted to create an Event, the Event Node would be all 
        the information that the user can input through the Event form about that specific event.  Each time a new event is created it is assigned a unique Node ID, which 
        is used to differentiate Events or nodes.  In the case of events, the database can collect unlimited events, or event nodes, each one being unique, and just as Events 
        have nodes, so do each of the other Content Types.</p>
      </li>
      <li>
        <strong>Roles (Drupal)</strong>
        <p>(Drupal) Roles are the names of the types of Users with respect to the web site and Drupal, not EPSCoR.  A Drupal role is defined by the site 
        administrator and represent the various groups of people that will be using the site.  For each role privileges will be assigned giving or restricting access to various 
        functions of the site.  Typical Drupal Roles are Adminsitrator, EPSCoR Administrator, Faculty, Student, Committee Member and Public.  This approach allows new Users to 
        be assigned appropriate access to the site quickly and confidently.  NSF defined (EPSCoR) roles are different and often confused. </p>
      </li>
      <li>
        <strong>User Profile</strong>
        <p>The User Profile is the data or information that is associated with a User.  It is in the User Profile where the User's real name, user name, 
        password, photo, EPSCoR roles, involvement, contact information, demographics, etc., is kept.</p>
      </li>
      <li>
        <strong>User Privileges</strong>
        <p>User privileges are the set of access or restrictions to the various functions of the site.  For each content type one can define who can see, 
        edit, delete or create new and whether or not you need to be logged in or not to do so.  User privileges are grouped by roles to allow the site administrator to quickly 
        assign or restrict access to common types of users.</p>
      </li>
      <li>
        <strong>View</strong>
        <p>A View is the web page that displays a node, or the information or data that has been input through a form.  Typically after you enter data or information 
        through a form and click Save, the View will follow allowing you to see what you just entered.</p>
      </li>
      <li>
        <strong>Widget</strong>
        <p>A widget is a generic term that describes items on a form that does a specific function.  A common widget is the calendar widget.  The calendar widget has 
      	both the little fields for day, month and year, where you can enter that data manually or you can click a day on a mini-calendar.</p>
      </li>
    </ul>
	</div>
	<h3 id="definitions">
		<a href="#definitions">Key Term NSF Definitions and the Criteria in the System that Defines these Terms</a>
	</h3>
	<div>
    <p>The following are a few key terms used in EPSCoR reporting, which may or may not be defined by NSF, yet must be and have been defined by the 
    ER-Core collaborative group in order for the Drupal ER-Core to properly handle this data.  EPSCoR administrators must understand the ER-Core criterion in order to 
    have confidence in the data and manage it properly.</p>
    <ul>
      <li>
        <strong>Participant (NSF)</strong>
        <p>Participants are members of the project who contribute in an ongoing and regular basis. They can be paid or unpaid and are involved in 
        activities funded by the project.</p>
        <p>Note, in the absence of a formal NSF definition of a Participant of EPSCoR, the ER-Core Drupal Committee defines a Participant as:</p>
        <p>
          <ul>
            <li>Needs to be housed at one of the partnering institutions as defined by the RII, and,</li>
            <li>Contribute equal to or greater than 160 hours (one person month) in a year (reporting period).</li>
          </ul>
        </p>
      </li>
      <li>
        <strong>Participant (ER-Core)</strong>
        <p>Once the user's account meets the following criteria, the system adds 1 to the appropriate category in Table A - Participants.  The criteria is: </p>
        <p>
          <ul>
            <li>User must be marked as Active</li>
            <li>User must have the "Authenticated EPSCoR" user role</li>
            <li>User must belong to a Participating Institution</li>
            <li>User must contribute at least 160 hours per year (1 person-month of effort)</li>
          </ul>
        </p>
      </li>
      <li>
        <strong>Participating Institution (NSF)</strong>
        <p>Participating institutions are those institutions that receive direct support from the RII award including sub-awards.</p>
      </li>
      <li>
        <strong>Participating Institution (ER-Core)</strong>
        <p>Institution node must be marked as participating (checkbox, an administrative function).  For each institution that has 
        its "participating" field checked, the system is then able to properly place participants in Table A, by institution.</p>
      </li>
      <li>
        <strong>Collaboration (NSF)</strong>
        <p>NSF does not formally define what a collaboration is for EPSCoR. Note, in the absence of a formal NSF definition of a collaboration 
        for EPSCoR, the ER-Core Drupal Committee defines a collaboration as a set of activities between a participating institution(s) and non-participating institutions, 
        organizations or agencies that lead to a definable outcome.  In a collaboration, each party contributes intellectually and brings complementary capabilities and resources. 
        Examples can include activities such as research projects, outreach events, database development or web dissemination of research data and results.</p>
      </li>
      <li>
        <strong>Collaboration (ER-Core)</strong>
        <p>A collaboration is the composition of 2 different elements.  Each of these elements have criteria that must be met.  Once all the 
        required elements meet the criteria, a collaboration is established and a count of 1 is added to Table C - Collaborations, regardless of how many EPSCoR researchers 
        participate in the collaboration.  The following is a list of the required elements for a collaboration:</p>
        <p>
          <ol>
            <li>Non-participating Institution (see above criteria to be counted as participating)</li>
            <li>Text fields that describe:</li>
            <ul>
              <li>Title</li>
              <li>Description of work and contribution to the RII</li>
              <li>Start Date</li>
              <li>Name of at least one EPSCoR participant</li>
              <li>Name of at least one EPSCoR research component or project</li>
              <li>Names of collaborators (see definition of collaborator below)</li>
              <li>Department for each named collaborator</li>
            </ul>
          </ol>
        </p>
      </li>
      <li>
        <strong>Collaborator (Drupal Committee)</strong>
        <p>NSF does not formally define who a collaborator is for EPSCoR. Note, in the absence of a formal NSF definition of who a 
        collaborator is for EPSCoR, the ER-Core Drupal Committee defines a collaborator as a researcher, faculty level or equivalent, who significantly contributes 
        (intellectually, labor) to the project less than 160 hrs/yr.  A collaborator may or may not be housed in a participating institution.  </p>
      </li>
      <li>
        <strong>Collaborator (ER-Core)</strong>
        <p>A collaborator contributes less than 160 hrs of effort during the (reporting period) year.  This value is set in the user profile within 
        the Involvement tab.  A collaborator may or may not be housed at a participating institution.</p>
        <p>Collaborator names are simply text fields and part of the Collaboration Content Type, not formal roles assigned within a default installation of the ER-Core.  For 
        each name that is listed as a collaborator in a completed collaboration, the system counts 1 and adds that to Table C - Collaborations.</p>
        <p>The default ER-Core role options do not include a Collaborator Role.  This is left up to the jurisdiction.  We suggest using the Guest Role to allow collaborators 
        access to the system.  </p>
      </li>
      <li>
        <strong>New Investigator (NSF)</strong>
        <p>New investigators are those at the faculty, junior faculty, and post doc level who are new to the RII project during the reporting period.</p>
      </li>
      <li>
        <strong>New Investigator (ER-Core)</strong>
        <p>New Investigators are Participants assigned with Post Doc or Faculty roles who's start date is less then 365 days from the most recent 
        reporting period.  For each user who's profile meets the below criteria, the system counts 1 and adds it to Table B - Participants.  </p>
        <p>In order for the system to count a user as a new investigator, the following criteria must be met:</p>
        <p>
          <ol>
            <li>Be an active system-defined participant</li>
            <li>Have the faculty or Post Doc role assigned</li>
            <li>Have the difference between the most recent reporting period and the user's start date be less than 365 days</li>
          </ol>
        </p>
      </li>
      <li>
        <strong>Project Start Date (ER-Core)</strong>
        <p>The start date is set by administration after the installation of the ER-Core in the EPSCoR Reporting Module Settings page accessed from 
        the black menu (default install) under Configuration/EPSCoR/EPSCoR Reporting Module Settings.  It is arbitrarily set and can be changed at any point.</p>
      </li>
      <li>
        <strong>Reporting Period (ER-Core)</strong>
        <p>The Reporting Period uses the Year of the Start Date, plus a month (arbitrarily set by administration), and the first day of that month.  
        The Reporting Period starting month is set in the EPSCoR Reporting Module Settings page accessed from the black menu (default install) under Configuration/EPSCoR/EPSCoR 
        Reporting Module Settings.  The Reporting Period starting day is 1 and cannot be changed.</p>
      </li>
    </ul>
	</div>

</div>
<script>
  //(function name($) {
  //  var $help = $("#er-help").accordion({
  //    autoHeight: false, //the current docs say to use "heightStyle", this is old jquery ui
  //    active: window.location.hash,
  //    collapsible: true
  //  });
  //  $("h3 > a").click(function(event){
  //    window.location.hash = this.hash;
  //  });
  //})(jQuery);
</script>