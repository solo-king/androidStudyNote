涉及指令：
	logcat SettingsDrawerActivity:V Settings:V DashboardSummary:V TileUtils:V *:S&
	su
	mount -o rw,remount /system 
	cp /sdcard/Settings.apk /system/priv-app/Settings/
	chmod 644 /system/priv-app/Settings/Settings.apk
	sync
	reboot

基础知识：
	ViewGroup动态加载：
			//R.layout.settings_main_dashboard
			<FrameLayout xmlns:android="http://schemas.android.com/apk/res/android"
				 android:id="@+id/main_content"
				 android:layout_height="match_parent"
				 android:layout_width="match_parent"
				 android:background="@color/material_grey_300"
				 />
		mContent = (ViewGroup) findViewById(R.id.main_content);
	Intent
		显示：
			Intent intent = new Intent(QuizActivity.this, CheatActivity.class);
			startActivity(intent);
		发送数据：
			intent.putExtra("action", "delete");
		获取数据:
			mIntent = getIntent();
		    String action = mIntent.getStringExtra("action");
	
	ViewGroups等概念以及使用
		setContentView(mIsShowingDashboard ? R.layout.settings_main_dashboard : R.layout.settings_main_prefs);
		mContent = (ViewGroup) findViewById(R.id.main_content);
	Fragment布局
		//Activity	CrimeActivity.java
		public class CrimeActivity extends AppCompatActivity {
				@Override
				protected void onCreate(Bundle savedInstanceState) {
					super.onCreate(savedInstanceState);
					setContentView(R.layout.activity_crime);
					FragmentManager fm = getSupportFragmentManager();
					Fragment fragment = fm.findFragmentById(R.id.fragment_container);
					if (fragment == null) {
						fragment = new CrimeFragment();
						fm.beginTransaction()
						.add(R.id.fragment_container, fragment)
						.commit();
					}
				}
			}
		//布局文件  activity_crime.xml
			<FrameLayout
				xmlns:android="http://schemas.android.com/apk/res/android"
				android:id="@+id/fragment_container"
				android:layout_width="match_parent"
				android:layout_height="match_parent"/>
		//帧布局文件 fragment_crime.xml
			<LinearLayout xmlns:android="http://schemas.android.com/apk/res/android"
				android:layout_width="match_parent"
				android:layout_height="match_parent"
				android:layout_margin="16dp"
				android:orientation="vertical">
				<TextView
				style="?android:listSeparatorTextViewStyle"
				android:layout_width="match_parent"
				android:layout_height="wrap_content"
				android:text="@string/crime_title_label"/>
				<EditText
				android:id="@+id/crime_title"
				android:layout_width="match_parent"
				android:layout_height="wrap_content"
				android:hint="@string/crime_title_hint"/>
				<TextView
				style="?android:listSeparatorTextViewStyle"
				android:layout_width="match_parent"
				android:layout_height="wrap_content"
				android:text="@string/crime_details_label"/>
				<Button
				android:id="@+id/crime_date"
				android:layout_width="match_parent"
				android:layout_height="wrap_content"/>
				<CheckBox
				android:id="@+id/crime_solved"
				android:layout_width="match_parent"
				android:layout_height="wrap_content"
				android:text="@string/crime_solved_label"/>
			</LinearLayout>
			//			CrimeFragment.java
				@Override
				public View onCreateView(LayoutInflater inflater, ViewGroup container,
				Bundle savedInstanceState) {
				View v = inflater.inflate(R.layout.fragment_crime, container, false);
				mTitleField = (EditText)v.findViewById(R.id.crime_title);
				mTitleField.addTextChangedListener(new TextWatcher() {
					@Override
					public void beforeTextChanged(
						CharSequence s, int start, int count, int after) {
						// This space intentionally left blank
						}
						@Override
						public void onTextChanged(
						CharSequence s, int start, int before, int count) {
						mCrime.setTitle(s.toString());
						}
						@Override
						public void afterTextChanged(Editable s) {
						// This one too
						}
						});
					return v;
				}
	Fragment切换：
		Fragment f = Fragment.instantiate(this, fragmentName, args);
		FragmentTransaction transaction = getFragmentManager().beginTransaction();//FragmentManger
		transaction.replace(R.id.main_content, f);
		transaction.commitAllowingStateLoss();
		getFragmentManager().executePendingTransactions()	
	ViewPager：
		//CrimePagerActivity.java
		public class CrimePagerActivity extends AppCompatActivity {
			private ViewPager mViewPager;
			private List<Crime> mCrimes;
			@Override
			protected void onCreate(Bundle savedInstanceState) {
					super.onCreate(savedInstanceState);
					setContentView(R.layout.activity_crime_pager);
					
					mViewPager = (ViewPager) findViewById(R.id.crime_view_pager);
					mCrimes = CrimeLab.get(this).getCrimes();
					FragmentManager fragmentManager = getSupportFragmentManager();
					//设置适配器
					mViewPager.setAdapter(new FragmentStatePagerAdapter(fragmentManager) {
						@Override
						public Fragment getItem(int position) {
						Crime crime = mCrimes.get(position);
							return CrimeFragment.newInstance(crime.getId());
						}
						@Override
						public int getCount() {
							return mCrimes.size();
						}
					});
				}
		}
		//activity_crime_pager.xml
		<android.sipport.v4.view.ViewPager
			...
			android:id="@+id/crime_view_pager"
			...
		>
		
		谷歌官方文档：
			<!-- activity_screen_slide.xml -->
			<android.support.v4.view.ViewPager
				xmlns:android="http://schemas.android.com/apk/res/android"
				android:id="@+id/pager"
				android:layout_width="match_parent"
				android:layout_height="match_parent" />
		网络资源：
		//activty_simple_viewpager.xml
			<android.sipport.v4.view.ViewPager
				...
				android:id="@+id/simpleviewpager"
				...
			>
		//SimplerViewPagerActivity.java
			ViewPager mSimpleviewpager;
			setContentView(R.layout.activty_simple_viewpager);
			mSimpleviewpager = (ViewPager)findViewById(R.id.simpleviewpager);
			SimpleViewpagerAdapter xxxAdapter = new xxx;
			mSimpleviewpager.setAdapter(xxxAdapter);
	
	mViewPager = (RtlCompatibleViewPager) content.findViewById(R.id.pager);
        mPagerAdapter = new DashboardViewPagerAdapter(getContext(),
                getChildFragmentManager(), mViewPager);
        mViewPager.setAdapter(mPagerAdapter);
        mViewPager.addOnPageChangeListener(
                new TabChangeListener((SettingsActivity) getActivity()));

	ProfileSettingsPreferenceFragment

	RecyclerView使用：
		//CrimeLab.java
		public class CrimeLab {
			private static CrimeLab sCrimeLab;
			private List<Crime> mCrimes;

			public static CrimeLab get(Context context) {
				if (sCrimeLab == null) {
					sCrimeLab = new CrimeLab(context);
				}
				return sCrimeLab;
			}

			private CrimeLab(Context context) {
				mCrimes = new ArrayList<>();
				for (int i = 0; i < 100; i++) {
					Crime crime = new Crime();
					crime.setTitle("Crime #" + i);
					crime.setSolved(i % 2 == 0); // Every other one
					mCrimes.add(crime);
				}
			}

			public List<Crime> getCrimes() {
				return mCrimes;
			}
			public Crime getCrime(UUID id) {
				for (Crime crime : mCrimes) {

					if (crime.getId().equals(id)) {
						return crime;
					}	
				}
				return null;
			}
		}
		//activity_fragment.xml
		<?xml version="1.0" encoding="utf-8"?>
		<FrameLayout xmlns:android="http://schemas.android.com/apk/res/android"
			android:id="@+id/fragment_container"
			android:layout_width="match_parent"
			android:layout_height="match_parent"
		/>
		//CriminalIntent/SingleFragmentActivity.java ,将SingleFragmentActivity类设置为通用类
		public abstract class SingleFragmentActivity extends AppCompatActivity {
			protected abstract Fragment createFragment();
			@Override
			protected void onCreate(Bundle savedInstanceState) {
				super.onCreate(savedInstanceState);
				setContentView(R.layout.activity_fragment);
				FragmentManager fm = getSupportFragmentManager();
				Fragment fragment = fm.findFragmentById(R.id.fragment_container);

				if (fragment == null) {
					fragment = createFragment();

					fm.beginTransaction()
					.add(R.id.fragment_container, fragment)
					.commit();
				}
			}
		}
		//CrimeActivity.java
		public class CrimeActivity extends SingleFragmentActivity {

			@Override
			protected Fragment createFragment() {
				return new CrimeFragment();
			}
		}
		//CrimeListActivity
		public class CrimeListActivity extends SingleFragmentActivity {
			@Override
			protected Fragment createFragment() {

				return new CrimeListFragment();

			}
		}
		//list_item_crime.xml
		<?xml version="1.0" encoding="utf-8"?>
		<LinearLayout xmlns:android="http://schemas.android.com/apk/res/android"
			android:orientation="vertical" android:layout_width="match_parent"
			android:layout_height="wrap_content">

			<TextView
				android:id="@+id/crime_title"
				android:layout_width="match_parent"
				android:layout_height="wrap_content"
				android:text="Crime Title" />

			<TextView
				android:id="@+id/crime_date"
				android:layout_width="match_parent"
				android:layout_height="wrap_content"
				android:text="Crime Date"/>

		</LinearLayout>
		//fragment_crime_list.xml
		<android.support.v7.widget.RecyclerView
		xmlns:android="http://schemas.android.com/apk/res/android"
			android:id="@+id/crime_recycler_view"
			android:layout_width="match_parent"
			android:layout_height="match_parent"
		/>
		//CrimeListFragment.java
		public class CrimeListFragment extends Fragment {

			public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

				View view = inflater.inflate(R.layout.fragment_crime_list, container, false);
				mCrimeRecyclerView = (RecyclerView) view.findViewById(R.id.crime_recycler_view);
				//RecyclerView类 不 会 亲 自 摆 放 屏 幕 上 的 列 表 项 。 实 际 上 ， 摆 放 的 任 务 被 委 托 给 了LayoutManager
				mCrimeRecyclerView.setLayoutManager(new LinearLayoutManager(getActivity()));
				//主要将Adapter与RecyclerView进行关联
				updateUI();

				return view;
			}

			private void updateUI() {
				//单例模式
				CrimeLab crimeLab = CrimeLab.get(getActivity());
				List<Crime> crimes = crimeLab.getCrimes();

				mAdapter = new CrimeAdapter(crimes);
				mCrimeRecyclerView.setAdapter(mAdapter);
			}
			//内部类:CrimeHolder.java
			private class CrimeHolder extends RecyclerView.ViewHolder 
					implements View.OnClickListener{

				private TextView mTitleTextView;
				private TextView mDateTextView;

				private Crime mCrime;
				public CrimeHolder(LayoutInflater inflater, ViewGroup parent) {

					super(inflater.inflate(R.layout.list_item_crime, parent, false));
					itemView.setOnClickListener(this);
					mTitleTextView = (TextView) itemView.findViewById(R.id.crime_title);
					mDateTextView = (TextView) itemView.findViewById(R.id.crime_date);
				}

				public void bind(Crime crime) {
					mCrime = crime;
					mTitleTextView.setText(mCrime.getTitle());
					mDateTextView.setText(mCrime.getDate().toString());
				}

				@Override
				public void onClick(View view) {
					Toast.makeText(getActivity(),
					mCrime.getTitle() + " clicked!", Toast.LENGTH_SHORT)
					.show();
				}
			}
			//内部类:CrimeAdapter.java
			private class CrimeAdapter extends RecyclerView.Adapter<CrimeHolder> {
					private List<Crime> mCrimes;

					public CrimeAdapter(List<Crime> crimes) {
					mCrimes = crimes;
				}

				@Override
				public CrimeHolder onCreateViewHolder(ViewGroup parent, int viewType) {

					LayoutInflater layoutInflater = LayoutInflater.from(getActivity());
					return new CrimeHolder(layoutInflater, parent);
				}

				/*
					任何时候，只要有可能，都要确保onBindViewHolder方法轻巧、高效
				*/
				@Override
				public void onBindViewHolder(CrimeHolder holder, int position) {
					Crime crime = mCrimes.get(position);
					holder.bind(crime);
				}

				@Override
				public int getItemCount() {
					return mCrimes.size();
				}
			}
		}
		//ViewHolder:
			RecyclerView的任务仅限于回收和定位屏幕上的View
		//RecyclerView
			的任务仅限于回收和定位屏幕上的View。
		

			
		
源码分析：
	//DeviceInfoSettings.java 加载
		 addPreferencesFromResource(R.xml.device_info_settings);
			//删除节点,KEY_SELINUX_STATUS = "selinux_status" ,PROPERTY_SELINUX_STATUS = "ro.build.selinux"
				removePreferenceIfPropertyMissing(getPreferenceScreen(), KEY_SELINUX_STATUS, PROPERTY_SELINUX_STATUS);
			//设置关键点击事件
				findPreference(KEY_FIRMWARE_VERSION).setEnabled(true);
		Legal information加载分析:
			//string.xml
			<string name="legal_information">Legal information</string>
			<string name="wallpaper_attributions">Wallpapers</string>
			<string name="wallpaper_attributions_values">Satellite imagery providers:\n?2014 CNES / Astrium, DigitalGlobe, Bluesky</string>
			//device_info_settings.xml
			<!-- Legal Information -->
			<PreferenceScreen
					android:key="container"
					android:title="@string/legal_information"
					android:fragment="com.android.settings.LegalSettings" />
			//about_legal.xml
			<!-- Open source licenses -->
			  <Preference
				android:key="wallpaper_attributions"
				android:title="@string/wallpaper_attributions"
				android:summary="@string/wallpaper_attributions_values"
				/>
	About tablet加载分析：
		
入口分析:
	//SettingsActivity.java
	onCreate
		//加载主界面，为一个fragment
		setContentView(mIsShowingDashboard ? R.layout.settings_main_dashboard : R.layout.settings_main_prefs);
			//切换到DashboardContainerFragment
			switchToFragment(DashboardContainerFragment.class.getName(), args, false, false,mInitialTitleResId, mInitialTitle, false);
				//DashboardContainerFragment.java
					onCreateView
						mViewPager = (RtlCompatibleViewPager) content.findViewById(R.id.pager);
						mPagerAdapter = new DashboardViewPagerAdapter(getContext(),getChildFragmentManager(), mViewPager);
						mViewPager.setAdapter(mPagerAdapter);
问题点：
	Settings.java中添加:
	public static class UsrSettings    extends SettingsActivity { /* add by chenqigan */ }
	//AndroidManifest.xml
	 <activity android:name=".Settings$UsrSettings"
            android:label="@string/header_category_usr">
            <intent-filter android:priority="5">
                <action android:name="com.android.settings.category.usr" />
                <category android:name="android.intent.category.DEFAULT" />
            </intent-filter>
        </activity>

	有人设置:
	//AndroidManifest.xml
	 <activity android:name=".Settings$WirelessSettings"
            android:label="@string/header_category_usr">
            <intent-filter android:priority="5">
                <action android:name="com.android.settings.category.usr" />
                <category android:name="android.intent.category.DEFAULT" />
            </intent-filter>
        </activity>

		<activity android:name="Settings$UsrNetworkInfoActivity"
                android:label="@string/data_usage_summary_title"
                android:icon="@drawable/ic_settings_data_usage"
                android:taskAffinity="">
            <intent-filter android:priority="1">
                <action android:name="com.android.settings.action.SETTINGS" />
            </intent-filter>
            <meta-data android:name="com.android.settings.category"
                android:value="com.android.settings.category.usr" />
            <meta-data android:name="com.android.settings.FRAGMENT_CLASS"
                android:value="com.android.settings.datausage.DataUsageSummary" />
        </activity>
		//添加datausage/UsrNetworkInfoFragment.java
		//Settings.java
		public static class UsrNetworkInfoActivity extends SettingsActivity{ /* empty */ }
		 //SettingsActivity.java
		 private String[] SETTINGS_FOR_RESTRICTED = {
			 ...
			Settings.UsrNetworkInfoActivity.class.getName(),
			 ...
		 }
		private void doUpdateTilesList()
			...
			setTileEnabled(new ComponentName(packageName,
						Settings.UsrNetworkInfoActivity.class.getName()),
						pm.hasSystemFeature(PackageManager.FEATURE_PRINTING), isAdmin, pm);
			...
		private static final String[] ENTRY_FRAGMENTS = {
				...
			UsrNetworkInfoFragment.class.getName(),
				..
		
	无线：	
	 <!-- wireless&network -->
	 <activity android:name=".Settings$WirelessSettings"
            android:label="@string/header_category_wireless_networks">
            <intent-filter android:priority="4">
                <action android:name="com.android.settings.category.wireless" />
                <category android:name="android.intent.category.DEFAULT" />
            </intent-filter>
        </activity>
		<!-- Data uage -->
		<activity android:name="Settings$DataUsageSummaryActivity"
                android:label="@string/data_usage_summary_title"
                android:icon="@drawable/ic_settings_data_usage"
                android:taskAffinity="">
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.DEFAULT" />
                <category android:name="com.android.settings.SHORTCUT" />
            </intent-filter>
            <intent-filter android:priority="1">
                <action android:name="com.android.settings.action.SETTINGS" />
            </intent-filter>
            <meta-data android:name="com.android.settings.category"
                android:value="com.android.settings.category.wireless" />
            <meta-data android:name="com.android.settings.FRAGMENT_CLASS"
                android:value="com.android.settings.datausage.DataUsageSummary" />
        </activity>

	2.何时加载Settings category、dashboard
	3.如何加载Settings category、dashboard
		//AndroidManifest.xml
		  <activity android:name=".Settings$WirelessSettings"
            android:label="@string/header_category_usr">
            <intent-filter android:priority="5">
                <action android:name="com.android.settings.category.usr" />
                <category android:name="android.intent.category.DEFAULT" />
            </intent-filter>
	      </activity>
		 
	4.设置中添加Category及Tiles
		Usr Category添加:
			<activity android:name=".SystemSettings"
				android:label="@string/header_category_system">
				<intent-filter android:priority="1">
					<action android:name="com.android.settings.category.system" />
					<category android:name="android.intent.category.DEFAULT" />
				</intent-filter>
			 </activity>

		Usr Settings Tiles添加过程：
			 //Settings.java
			 public static class PrintSettingsActivity extends SettingsActivity { /* empty */ }
			 //SettingsActivity.java
			 private String[] SETTINGS_FOR_RESTRICTED = {
				 ...
				Settings.PrintSettingsActivity.class.getName(),
				 ...
			 }
			private void doUpdateTilesList()
				...
				setTileEnabled(new ComponentName(packageName,
							Settings.PrintSettingsActivity.class.getName()),
							pm.hasSystemFeature(PackageManager.FEATURE_PRINTING), isAdmin, pm);
				...
			private static final String[] ENTRY_FRAGMENTS = {
					...
				PrintSettingsFragment.class.getName(),
					..

			PrintServiceSettingsFragment.java
		//	AndroidManifest.xml
		  <activity android:name=".SystemSettings"
				android:label="@string/header_category_system">
				<intent-filter android:priority="1">
					<action android:name="com.android.settings.category.system" />
					<category android:name="android.intent.category.DEFAULT" />
				</intent-filter>
			</activity>

			<activity android:name="Settings$PrintSettingsActivity"
                android:label="@string/print_settings"
                android:icon="@drawable/ic_settings_print"
                android:taskAffinity="com.android.settings"
                android:parentActivityName="Settings">
            <intent-filter android:priority="1">
                <action android:name="android.settings.ACTION_PRINT_SETTINGS" />
                <category android:name="android.intent.category.DEFAULT" />
            </intent-filter>
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.DEFAULT" />
                <category android:name="android.intent.category.VOICE_LAUNCH" />
            </intent-filter>
            <intent-filter android:priority="3">
                <action android:name="com.android.settings.action.SETTINGS" />
            </intent-filter>
            <meta-data android:name="com.android.settings.category"
                android:value="com.android.settings.category.system" />
            <meta-data android:name="com.android.settings.FRAGMENT_CLASS"
                android:value="com.android.settings.print.PrintSettingsFragment" />

        </activity>
	5.About tablet 显示结构
		//AndroidManifest.xml
		 <activity android:name="Settings$DeviceInfoSettingsActivity"
                android:theme="@style/Theme.SubSettingsDialogWhenLarge"
                android:label="@string/device_info_settings"
                android:icon="@drawable/ic_settings_about"
                android:taskAffinity="com.android.settings"
                android:parentActivityName="Settings">
            <intent-filter android:priority="1">
                <action android:name="android.settings.DEVICE_INFO_SETTINGS" />
                <category android:name="android.intent.category.DEFAULT" />
            </intent-filter>
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.DEFAULT" />
                <category android:name="android.intent.category.VOICE_LAUNCH" />
            </intent-filter>
            <intent-filter android:priority="-1">
                <action android:name="com.android.settings.action.SETTINGS" />
            </intent-filter>
            <meta-data android:name="com.android.settings.category"
                android:value="com.android.settings.category.system" />
            <meta-data android:name="com.android.settings.title"
                android:resource="@string/about_settings" />	<!-- About tablet -->

            <meta-data android:name="com.android.settings.FRAGMENT_CLASS"
                android:value="com.android.settings.DeviceInfoSettings" />

            <meta-data android:name="com.android.settings.PRIMARY_PROFILE_CONTROLLED"
                android:value="true" />
        </activity>

	6.添加提示框
<!-- Date &amp; time -->
	<activity android:name="Settings$DateTimeSettingsActivity"
                android:label="@string/date_and_time"	
                android:icon="@drawable/ic_settings_date_time"
                android:taskAffinity="">
            <intent-filter android:priority="1">
                <action android:name="android.settings.DATE_SETTINGS" />
                <category android:name="android.intent.category.DEFAULT" />
            </intent-filter>
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <action android:name="android.intent.action.QUICK_CLOCK" />
                <category android:name="android.intent.category.VOICE_LAUNCH" />
                <category android:name="android.intent.category.DEFAULT" />
            </intent-filter>
            <intent-filter android:priority="5">
                <action android:name="com.android.settings.action.SETTINGS" />
            </intent-filter>
            <meta-data android:name="com.android.settings.category"
                android:value="com.android.settings.category.system" />
            <meta-data android:name="com.android.settings.FRAGMENT_CLASS"
                android:value="com.android.settings.DateTimeSettings" />
            <meta-data android:name="com.android.settings.PRIMARY_PROFILE_CONTROLLED"
                android:value="true" />
        </activity>
<!--Accessibility-->
		<activity android:name="Settings$AccessibilitySettingsActivity"
                android:label="@string/accessibility_settings"	
                android:icon="@drawable/ic_settings_accessibility"
                android:configChanges="orientation|keyboardHidden|screenSize"
                android:taskAffinity="">
            <intent-filter android:priority="1">
                <action android:name="android.settings.ACCESSIBILITY_SETTINGS" />
                <category android:name="android.intent.category.DEFAULT" />
            </intent-filter>
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.DEFAULT" />
                <category android:name="android.intent.category.VOICE_LAUNCH" />
                <category android:name="com.android.settings.SHORTCUT" />
            </intent-filter>
            <intent-filter android:priority="4">
                <action android:name="com.android.settings.action.SETTINGS" />
            </intent-filter>
            <meta-data android:name="com.android.settings.category"
                android:value="com.android.settings.category.system" />
            <meta-data android:name="com.android.settings.FRAGMENT_CLASS"
                android:value="com.android.settings.accessibility.AccessibilitySettings" />
            <meta-data android:name="com.android.settings.PRIMARY_PROFILE_CONTROLLED"
                android:value="true" />
        </activity>

<!--About tablet-->
		<activity android:name="Settings$DeviceInfoSettingsActivity"
                android:theme="@style/Theme.SubSettingsDialogWhenLarge"
                android:label="@string/device_info_settings"
                android:icon="@drawable/ic_settings_about"
                android:taskAffinity="com.android.settings"
                android:parentActivityName="Settings">
            <intent-filter android:priority="1">
                <action android:name="android.settings.DEVICE_INFO_SETTINGS" />
                <category android:name="android.intent.category.DEFAULT" />
            </intent-filter>
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.DEFAULT" />
                <category android:name="android.intent.category.VOICE_LAUNCH" />
            </intent-filter>
            <intent-filter android:priority="-1">
                <action android:name="com.android.settings.action.SETTINGS" />
            </intent-filter>
            <meta-data android:name="com.android.settings.category"
                android:value="com.android.settings.category.system" />
            <meta-data android:name="com.android.settings.title"
                android:resource="@string/about_settings" />
            <meta-data android:name="com.android.settings.FRAGMENT_CLASS"
                android:value="com.android.settings.DeviceInfoSettings" />
            <meta-data android:name="com.android.settings.PRIMARY_PROFILE_CONTROLLED"
                android:value="true" />
        </activity>

<!--Printing-->
		<activity android:name="Settings$PrintSettingsActivity"
                android:label="@string/print_settings"	//title
                android:icon="@drawable/ic_settings_print"
                android:taskAffinity="com.android.settings"
                android:parentActivityName="Settings">
            <intent-filter android:priority="1">
                <action android:name="android.settings.ACTION_PRINT_SETTINGS" />
                <category android:name="android.intent.category.DEFAULT" />
            </intent-filter>
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.DEFAULT" />
                <category android:name="android.intent.category.VOICE_LAUNCH" />
            </intent-filter>
            <intent-filter android:priority="3">
                <action android:name="com.android.settings.action.SETTINGS" />
            </intent-filter>
            <meta-data android:name="com.android.settings.category"
                android:value="com.android.settings.category.system" />

            <meta-data android:name="com.android.settings.FRAGMENT_CLASS"
                android:value="com.android.settings.print.PrintSettingsFragment" />

        </activity>

    private static final String SETTINGS_ACTION =
            "com.android.settings.action.SETTINGS";

    private static final String OPERATOR_SETTINGS =
            "com.android.settings.OPERATOR_APPLICATION_SETTING";

	private static final String MANUFACTURER_SETTINGS =
            "com.android.settings.MANUFACTURER_APPLICATION_SETTING";

   private static final String EXTRA_SETTINGS_ACTION =
            "com.android.settings.action.EXTRA_SETTINGS";

<!--Usr Settings-->
		<activity android:name="Settings$PrintSettingsActivity"
                android:label="@string/usr_settings"	
                android:icon="@drawable/ic_settings_print"
                android:taskAffinity="com.android.settings"
                android:parentActivityName="Settings">
            <intent-filter android:priority="6">
                <action android:name="com.android.settings.action.SETTINGS" />
            </intent-filter>
            <meta-data android:name="com.android.settings.category"
                android:value="com.android.settings.category.system" />
            <meta-data android:name="com.android.settings.FRAGMENT_CLASS"
                android:value="com.android.settings.print.PrintSettingsFragment" />
        </activity>


1.底板




	
