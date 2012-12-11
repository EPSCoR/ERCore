<h2>Installation Customizations</h2>
<p>
  <strong>
  These are steps that should be taken immediately after installing the EPSCoR Reporting package.
  These customizations will help make the system a better fit your juristiction.
  </strong>
</p>

<h4>Project Components</h4>
<p>
  Change the list of <?php echo l('project components', 'admin/structure/taxonomy/er_components')?> (which may also be known as "teams").
  Use the "Add term" button to add a new component.
  This will present you with a form, which includes a checkbox labeled "Core Component",
  this checkbox denotes whether the component should show up on every component selection list, or just on the user profile's select list.
</p>
<p>
  Ex. The "Administration" component is non-core, while "CYBER", "ENDER", "ECOGEM", and "DEW" are all core teams for Hawaii.
</p>

<h4>Customize the list of ethnicities/races</h4>
<p>
  Modify the <?php echo l('list of ethnicities/races', 'admin/config/people/accounts/fields/field_er_race')?> that your jurisdiction will want to track and report on.
</p>
<p>
  Ex. In Hawaii we also provide options for: Filipino, Pacific Islander, and Native Hawaiian
</p>

<h4>Committees/Advisory Boards</h4>
<p>
  Change the <?php echo l('list of Committees/Advisory Boards', 'admin/config/people/accounts/fields/field_er_committees')?> which oversee your EPSCoR grant.
</p>
<p>
  Ex. Hawaii has three advisory committees: Community Advisory Committee (CAC), Monitoring Assessment Panel (MAP), Statewide Committee (SWC)  
</p>

<h4>Institutions</h4>
<p>
  The reporting system needs to know certain information about the institutions being attached to users, or collaborations.
  We need to know where the users come from, and what kind of institution it is in order to automatically fulfill certain NSF requirements.
</p>
<p>
  The system requires that you explicitly list the "Participating Institutions" (i.e. Those institutions which are participating in your EPSCoR grant), as well as any institution which will be listed in a collaboration.
  This list will likely expand over time, but it is necessary to begin this process by setting up the most important institutions first.
</p>
<p>
  You can add a new institution to the system by going to the <?php echo l('Add content', 'node/add')?> link, then selecting the <?php echo l('Institution', 'node/add/er-institution')?> link.
</p>
<p>
  You may also see a list of institutions currently in the system through the <?php echo l('Institutions view', 'institutions')?>.  
</p>

<h4>Data Repository Managers</h4>
<p>
  After setting up user accounts for your administrators, you will be able to flag users whom should receive email notifications regarding researcher data collection events.
  We suggest that you create an account for your Data Repository Manager, then flag this person's account to recieve the notifications. Other users may also be flagged to recieve these notifications using the same procedure.
</p>
<p>
  In order to flag the user, you must first navigate to their user profile.
  You can find the user profile of someone in the system by navigating to the <?php echo l('People', 'admin/people')?> admin page.
</p>
<p>
  Once viewing the user's profile, scroll to the bottom of the page, and locate the blue link labeled "Receive data collection notifications" and click on that link.
  The link should change it's label to say "Do not receive data collection notifications", which when clicked will remove the user from recieving these notifications.
</p>

<h4>Start date of grant and reporting periods</h4>
<p>
  Visit the <?php echo l('ER-Core Settings page', 'admin/config/epscor/er')?>, and change the start date.
</p>

<h2>Quick-Start Guide</h2>
<h4>Do some things</h4>
<p>
  TODO
</p>
