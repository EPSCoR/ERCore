<h2>Administrative Guide</h2>
<div>
  <p>
    This guide is aimed at site administrators and administrative staff. This document helps describe some of the internal features of ER Core, and how administrators may customize the Core to meet jurisdictional needs.
    This document aims to describe the various tasks, views, operations, and procedures that an administrator may need to know about in order to properly administer the site.
  </p>
  <p>You may also be interested in the more general <strong><?php echo l('User Guide', 'admin/epscor/user-guide'); ?></strong> which talks more about how researchers should enter data into the system.</p>
</div>
<div id="er-help-admin">
  <h3 id="post-install"><a href="#post-install">Post-Installation Guide/Jurisdictional Customizations</a></h3>
  <div>
    <strong>
    These are steps that should be taken immediately after installing the EPSCoR Reporting package.
    These customizations will help make the system a better fit your juristiction.
    </strong>
    <ul>
      <li>
        <h4>Project Components</h4>
        <p>
          Change the list of <?php echo l('project components', 'admin/structure/taxonomy/er_components')?> (which may also be known as "teams").
          Use the "Add term" button to add a new component.
          This will present you with a form, which includes a checkbox labeled "Core Component",
          this checkbox denotes whether the component should show up on every component selection list, or just on the user profile's select list.
          <br>
          Ex. The "Administration" component is non-core, while "CYBER", "ENDER", "ECOGEM", and "DEW" are all core teams for Hawaii.
        </p>
      </li>
      <li>
        <h4>Customize the list of ethnicities/races</h4>
        <p>
          Modify the <?php echo l('list of ethnicities/races', 'admin/config/people/accounts/fields/field_er_race')?> that your jurisdiction will want to track and report on.
          <br>
          Ex. In Hawaii we also provide options for: Filipino, Pacific Islander, and Native Hawaiian
        </p>
      </li>
      <li>
        <h4>Committees/Advisory Boards</h4>
        <p>
          Change the <?php echo l('list of Committees/Advisory Boards', 'admin/config/people/accounts/fields/field_er_committees')?> which oversee your EPSCoR grant.
          <br>
          Ex. Hawaii has three advisory committees: Community Advisory Committee (CAC), Monitoring Assessment Panel (MAP), Statewide Committee (SWC)  
        </p>
      </li>
      <li>
        <h4>Institutions</h4>
        <p>
          The reporting system needs to know certain information about the institutions being attached to users, or collaborations.
          We need to know where the users come from, and what kind of institution it is in order to automatically fulfill certain NSF requirements.
          <br>
          The system requires that you explicitly list the "Participating Institutions" (i.e. Those institutions which are participating in your EPSCoR grant), as well as any institution which will be listed in a collaboration.
          This list will likely expand over time, but it is necessary to begin this process by setting up the most important institutions first.
          <br>
          You can add a new institution to the system by going to the <?php echo l('Add content', 'node/add')?> link, then selecting the <?php echo l('Institution', 'node/add/er-institution')?> link.
          <br>
          You may also see a list of institutions currently in the system through the <?php echo l('Institutions view', 'institutions')?>.  
        </p>
      </li>
      <li>
        <h4>Data Repository Managers</h4>
        <p>
          After setting up user accounts for your administrators, you will be able to flag users whom should receive email notifications regarding researcher data collection events.
          We suggest that you create an account for your Data Repository Manager, then flag this person's account to recieve the notifications. Other users may also be flagged to recieve these notifications using the same procedure.
          <br>
          In order to flag the user, you must first navigate to their user profile.
          You can find the user profile of someone in the system by navigating to the <?php echo l('People', 'admin/people')?> admin page.
          <br>
          Once viewing the user's profile, scroll to the bottom of the page, and locate the blue link labeled "Receive data collection notifications" and click on that link.
          The link should change it's label to say "Do not receive data collection notifications", which when clicked will remove the user from recieving these notifications.
        </p>
      </li>
      <li>
        <h4>Start date of grant and reporting periods</h4>
        <p>
          Visit the <?php echo l('ER-Core Settings page', 'admin/config/epscor/er')?>, and change the start date.
        </p>
      </li>
      <li id="phpexcel">
        <h4>Setup Excel Sheet Output</h4>
        <p>
          The system relies on an external library called <?php echo l('PHPExcel', 'http://phpexcel.codeplex.com/releases/view/107442')?> to export various excel sheets.
          The Library must be downloaded, then uploaded to this server.<br>
          <br>
          The PHPExcel kit should first be downloaded and decompressed, and the contents should then be uploaded to the server directory
          <code>/sites/all/libraries/PHPExcel/</code>, creating the <code>/libraries/PHPExcel/</code> directory if necessary.
          Once uploaded, <code>PHPExcel.php</code> should be located at <code>/sites/all/libraries/PHPExcel/Classes/PHPExcel.php</code>,
          the <code>IOFactory.php</code> should be located at <code>/sites/all/libraries/PHPExcel/Classes/PHPExcel/IOFactory.php</code>, and <code>changelog.txt</code> needs to be located at <code>/sites/all/libraries/PHPExcel/changelog.txt</code><br>
          <br>
          You can then navigate to the <?php echo l('ER-Core Settings page', 'admin/config/epscor/er')?> to see if the library was detected.
        </p>
      </li>
      <li>
        <h4>(Optional) Cutomize the list of Event Types</h4>
        <p>
          Some states may wish to <?php echo l('customize the list of "Event Types"', 'admin/structure/types/manage/er_event/fields/field_er_event_type', array('fragment' => 'edit-submit'))?> which label calendar events.<br>
          Notice: The available values should be of the form:<br>
          <code style="margin:10px;">#|Label</code><br>
          Where # is an integer value which is used as the internal representation (stored in the database), and Label is what is shown to the user.
        </p>
      </li>
      <li>
        <h4>(Optional) Public Profile settings</h4>
        <p>
          Some states may wish to hide the uer profiles from public view.<br>
          This may be accomplished by changing the "View user profiles" flag located on the <?php echo l('user permissions', 'admin/people/permissions/1', array('fragment' => 'edit-access-user-profiles'))?> page.
        </p>
      </li>
    </ul>
  </div>
</div>